// clang-format off
/*
# License
This software is distributed under two licenses, choose whichever you like.

## MIT License
Copyright (c) 2020-2021 Takuro Sakai

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Public Domain
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/
// clang-format on
/**
@author t-sakai
*/
#include "cppgltf.h"
#include <iterator>

namespace cppgltf
{
	//--- Buffer
	JsonParser::Buffer::Buffer(cppgltf_malloc allocator, cppgltf_free deallocator)
		:allocator_(allocator)
		, deallocator_(deallocator)
		, capacity_(0)
		, size_(0)
		, values_(nullptr)
	{
		CPPGLTF_ASSERT((nullptr != allocator && nullptr != deallocator) || (nullptr == allocator && nullptr == deallocator));
		if (nullptr == allocator_) {
			allocator_ = ::malloc;
		}
		if (nullptr == deallocator_) {
			deallocator_ = ::free;
		}
	}

	JsonParser::Buffer::~Buffer()
	{
		deallocator_(values_);
	}

	u32 JsonParser::Buffer::capacity() const
	{
		return capacity_;
	}

	u32 JsonParser::Buffer::size() const
	{
		return size_;
	}

	void JsonParser::Buffer::clear()
	{
		size_ = 0;
	}

	void JsonParser::Buffer::push_back(const Value& value)
	{
		if (capacity_ <= size_) {
			u32 capacity = capacity_ + 128;
			Value* values = reinterpret_cast<Value*>(allocator_(sizeof(Value) * capacity));
			if(nullptr != values_){
				::memcpy(values, values_, sizeof(Value)*capacity_);
			}
			deallocator_(values_);
			capacity_ = capacity;
			values_ = values;
		}
		values_[size_] = value;
		++size_;
	}

	const JsonParser::Value& JsonParser::Buffer::operator[](u32 index) const
	{
		CPPGLTF_ASSERT(index < size_);
		return values_[index];
	}

	JsonParser::Value& JsonParser::Buffer::operator[](u32 index)
	{
		CPPGLTF_ASSERT(index < size_);
		return values_[index];
	}

	//--- JsonParser
	JsonParser::JsonParser(cppgltf_malloc allocator, cppgltf_free deallocator, u32 max_nests)
		:allocator_(allocator)
		, deallocator_(deallocator)
		, max_nests_(max_nests)
		, nest_count_(0)
		, begin_(nullptr)
		, current_(nullptr)
		, end_(nullptr)
		, buffer_(allocator, deallocator)
	{
		CPPGLTF_ASSERT((nullptr != allocator && nullptr != deallocator) || (nullptr == allocator && nullptr == deallocator));
		if (nullptr == allocator_) {
			allocator_ = ::malloc;
		}
		if (nullptr == deallocator_) {
			deallocator_ = ::free;
		}
	}

	JsonParser::~JsonParser()
	{
	}

	bool JsonParser::parse(cursor head, cursor end)
	{
		CPPGLTF_ASSERT(head <= end);
		nest_count_ = 0;
		begin_ = current_ = head;
		end_ = end;
		buffer_.clear();
		skip_bom();
        skip_space();
        JsonParser::Result result = parse_value();
        skip_space();
        return result.success_ && end_<=current_;
	}

	u32 JsonParser::size() const
    {
		return buffer_.size();
    }

    const JsonParser::Value& JsonParser::getRoot() const
    {
		CPPGLTF_ASSERT(0<size());
		return buffer_[0];
    }

	void JsonParser::add_element_to_object(u32 object_index, u32 element_index)
	{
		CPPGLTF_ASSERT(object_index<buffer_.size());
		CPPGLTF_ASSERT(Type::Object == buffer_[object_index].type_);
		u32 prev_index = object_index;
		while(Invalid != buffer_[prev_index].next_){
			prev_index = buffer_[prev_index].next_;
		}
		buffer_[prev_index].next_ = element_index;
		buffer_[object_index].object_.size_ += 1;
	}

	void JsonParser::add_element_to_array(u32 array_index, u32 element_index)
	{
		CPPGLTF_ASSERT(array_index<buffer_.size());
		Value& value = buffer_[array_index];
		CPPGLTF_ASSERT(Type::Array == buffer_[array_index].type_);
		u32 prev_index = array_index;
		while(Invalid != buffer_[prev_index].next_){
			prev_index = buffer_[prev_index].next_;
		}
		buffer_[prev_index].next_ = element_index;
		buffer_[array_index].array_.size_ += 1;
	}

	void JsonParser::skip_bom()
	{
		size_t size = std::distance(current_, end_);
		if (3 <= size) {
			u8 c0 = *reinterpret_cast<const u8*>(&current_[0]);
			u8 c1 = *reinterpret_cast<const u8*>(&current_[1]);
			u8 c2 = *reinterpret_cast<const u8*>(&current_[2]);
			if (0xEFU == c0 && 0xBBU == c1 && 0xBFU == c2) {
				current_ += 3;
			}
		}
	}

	void JsonParser::skip_space()
	{
		while (current_ < end_) {
			switch (*current_) {
			case 0x09:
			case 0x0A:
			case 0x0D:
			case 0x20:
				++current_;
				break;
			default:
				return;
			}
		}
	}

	JsonParser::Result JsonParser::parse_value()
	{
		if (end_<=current_) {
			return {false, 0};
		}
		switch (*current_) {
		case '"':
			return parse_string();
		case '{':
			return parse_object();
		case '[':
			return parse_array();
		case 't':
			return parse_true();
		case 'f':
			return parse_false();
		case 'n':
			return parse_null();
		default:
			if('-' == *current_ || ('0'<=*current_ && *current_<='9')){
				return parse_number();
			}
			break;
		}
		return {false, 0};
	}

	JsonParser::Result JsonParser::parse_object()
	{
		CPPGLTF_ASSERT(current_ < end_ && '{' == *current_);
		++current_;
		if (end_ <= current_ || max_nests_<=nest_count_) {
			return {false, 0};
		}
		++nest_count_;
		u32 object_index = buffer_.size();
		{
			u32 index = buffer_.size();
			Value value;
			value.position_ = get_position();
			value.length_ = 0;
			value.next_ = Invalid;
			value.type_ = Type::Object;
			value.object_.size_ = 0;
			buffer_.push_back(value);
		}

		bool prev_begin = true;
		bool prev_comma = false;
		while (current_ < end_) {
			skip_space();
			u32 position = get_position();

			switch (*current_) {
			case '}':{
				if(prev_comma){
					return {false, 0};
				}
				buffer_[object_index].length_ = position - buffer_[object_index].position_ - 1;
				++current_;
				--nest_count_;
				return {true, object_index};
			}
			case '"':{
				prev_begin = false;
				prev_comma = false;
				Result key_result = parse_string();
				if(!key_result.success_){
					return key_result;
				}
				skip_space();
				if (end_ <= current_ || ':' != *current_) {
					return {false, 0};
				}
				++current_;
				skip_space();
				Result value_result = parse_value();
				if(!value_result.success_){
					return value_result;
				}
				Value key_value;
				key_value.position_ = position;
				key_value.length_ = buffer_[value_result.index_].position_ + buffer_[value_result.index_].length_ - position;
				key_value.next_ = Invalid;
				key_value.type_ = Type::KeyValue;
				key_value.key_value_.key_ = key_result.index_;
				key_value.key_value_.value_ = value_result.index_;
				u32 key_value_index = buffer_.size();
				buffer_.push_back(key_value);
				add_element_to_object(object_index, key_value_index);
				break;
			}
			case ',':
				if(prev_begin || prev_comma){
					return {false, 0};
				}
				prev_begin = false;
				prev_comma = true;
				++current_;
				break;
			default:
				return {false, 0};
			}
		}
		return {false, 0};
	}

	JsonParser::Result JsonParser::parse_array()
	{
		CPPGLTF_ASSERT(current_ < end_ && '[' == *current_);
		++current_;
		if (end_ <= current_ || max_nests_<=nest_count_) {
			return {false, 0};
		}
		++nest_count_;
		u32 array_index = buffer_.size();
		{
			Value value;
			value.position_ = get_position();
			value.length_ = 0;
			value.next_ = Invalid;
			value.type_ = Type::Array;
			value.array_.size_ = 0;
			buffer_.push_back(value);
		}

		bool prev_begin = true;
		bool prev_comma = false;
		while (current_ < end_) {
			skip_space();
			u32 position = get_position();

			switch (*current_) {
			case ']':{
				if(prev_comma){
					return {false, 0};
				}
				u32 length = position - buffer_[array_index].position_;
				buffer_[array_index].length_ = length;
				++current_;
				--nest_count_;
				return {true, array_index};
			}
			case ',':
				if(prev_begin){
					return {false, 0};
				}
				prev_begin = false;
				prev_comma = true;
				++current_;
				break;
			default:{
				if(!prev_begin && !prev_comma){
					return {false, 0};
				}
				Result result = parse_value();
				if(!result.success_){
					return result;
				}
				add_element_to_array(array_index, result.index_);
				prev_begin = false;
				prev_comma = false;
				break;
			}
			}
		}
		return {false, 0};
	}

	JsonParser::Result JsonParser::parse_string()
	{
		CPPGLTF_ASSERT(current_ < end_ && '"' == *current_);
		++current_;
		if (end_ <= current_) {
			return {false, 0};
		}
		u32 position = get_position();
		while (current_ < end_) {
			switch (*current_) {
			case '\\':{
				s32 result = parse_escape();
				if(result<=0){
					return {false, 0};
				}
				current_ += result;
				break;
			}
			case '"':{
				u32 size = get_position() - position;
				u32 index = buffer_.size();
				Value value;
				value.position_ = position;
				value.length_ = size;
				value.next_ = Invalid;
				value.type_ = Type::String;
				buffer_.push_back(value);
				++current_;
				return {true, index};
			}
			default: {
				s32 result = parse_utf8();
				if(result<=0){
					return {false, 0};
				}
				current_ += result;
				break;
			}
			}
		}
		return {false, 0};
	}

	JsonParser::Result JsonParser::parse_true()
	{
		CPPGLTF_ASSERT(current_ < end_ && 't' == *current_);
		if (end_ <= (current_+3)) {
			return {false, 0};
		}
		if('r' == current_[1]
			&& 'u' == current_[2]
			&& 'e' == current_[3]){
			u32 index = buffer_.size();
			Value value;
			value.position_ = get_position();
			value.length_ = 4;
			value.next_ = Invalid;
			value.type_ = Type::True;
			buffer_.push_back(value);
			current_ += 4;
			return {true, index};
		}
		return {false, 0};
	}

	JsonParser::Result JsonParser::parse_false()
	{
		CPPGLTF_ASSERT(current_ < end_ && 'f' == *current_);
		if (end_ <= (current_+4)) {
			return {false, 0};
		}
		if('a' == current_[1]
			&& 'l' == current_[2]
			&& 's' == current_[3]
			&& 'e' == current_[4]){
			u32 index = buffer_.size();
			Value value;
			value.position_ = get_position();
			value.length_ = 5;
			value.next_ = Invalid;
			value.type_ = Type::False;
			buffer_.push_back(value);
			current_ += 5;
			return {true, index};
		}
		return {false, 0};
	}

	JsonParser::Result JsonParser::parse_null()
	{
		CPPGLTF_ASSERT(current_ < end_ && 'n' == *current_);
		if (end_ <= (current_+3)) {
			return {false, 0};
		}
		if('u' == current_[1]
			&& 'l' == current_[2]
			&& 'l' == current_[3]){
			u32 index = buffer_.size();
			Value value;
			value.position_ = get_position();
			value.length_ = 4;
			value.next_ = Invalid;
			value.type_ = Type::Null;
			buffer_.push_back(value);
			current_ += 4;
			return {true, index};
		}
		return {false, 0};
	}

	JsonParser::Result JsonParser::parse_number()
	{
		CPPGLTF_ASSERT(current_ < end_);
		CPPGLTF_ASSERT('-' == *current_ || ('0'<=*current_ && *current_<='9'));
		bool minus = '-' == *current_;
		bool zero = '0' == *current_;
		u32 position = get_position();
		++current_;

		if(minus){
			if (end_ <= current_) {
				return {false, 0};
			}
			if(!('0'<=*current_ && *current_<='9')){
				return {false, 0};
			}
			zero = '0' == *current_;
			++current_;
		}
		if (end_ <= current_) {
			return get_number(position);
		}
		if(!zero){
			parse_integer();
			if (end_ <= current_) {
				return get_number(position);
			}
		}

		if('.' == *current_){
			if(parse_fraction()<0){
				return {false, 0};
			}
            if(end_ <= current_) {
                return get_number(position);
            }
		}
		if('E' == *current_ || 'e' == *current_){
			if(parse_exponent()<0){
				return {false, 0};
			}
		}
		return get_number(position);
	}

	s32 JsonParser::parse_escape()
	{
		CPPGLTF_ASSERT(current_ < end_ && '\\' == *current_);
		++current_;
		if (end_ <= current_) {
			return false;
		}
		switch(*current_){
		case '"':
		case '\\':
		case '/':
		case 'b':
		case 'f':
		case 'n':
		case 'r':
		case 't':
			return 1;
		case 'u':
			return parse_4hexdigit();
		default:
			return -1;
		}
	}

	s32 JsonParser::parse_utf8()
	{
		u8 u0 = *((u8*)current_);
		if (u0 <= 0x7FU) {
			return 0x20U<=u0? 1 : -1;
		}
		if (0xC2U <= u0 && u0 < 0xE0U) {
			if (end_ <= (current_ + 1)) {
				return -1;
			}
			return 2;
		}

		if (0xE0U <= u0 && u0 < 0xF0U) {
			if (end_ <= (current_ + 2)) {
				return -1;
			}
			return 3;
		}

		if (0xF0U <= u0 && u0 < 0xF8U) {
			if (end_ <= (current_ + 3)) {
				return -1;
			}
			return 4;
		}
		return -1;
	}

	s32 JsonParser::parse_4hexdigit()
	{
		CPPGLTF_ASSERT(current_ < end_ && 'u' == *current_);
		++current_;
		if (end_ <= current_) {
			return -1;
		}
		if(!is_hex_digit(*current_)){
			return -1;
		}

		if (end_ <= (current_+1)) {
			return -1;
		}
		if(!is_hex_digit(*(current_+1))){
			return -1;
		}

		if (end_ <= (current_+2)) {
			return -1;
		}
		if(!is_hex_digit(*(current_+2))){
			return -1;
		}

		if (end_ <= (current_+3)) {
			return -1;
		}
		if(!is_hex_digit(*(current_+3))){
			return -1;
		}
		return 4;
	}

	void JsonParser::parse_integer()
	{
		while(current_<end_){
            if('0' <= *current_ && *current_ <= '9') {
                ++current_;
            } else {
				return;
            }
		}
	}

	s32 JsonParser::parse_fraction()
	{
		CPPGLTF_ASSERT(current_ < end_ && '.' == *current_);
		++current_;
		if (end_ <= current_) {
			return -1;
		}
		if(!('0'<=*current_ && *current_<='9')){
			return -1;
		}

		s32 count = 0;
		while(current_<end_){
			if(!('0'<=*current_ && *current_<='9')){
				break;
			}
			++current_;
			++count;
		}
		return count;
	}

	s32 JsonParser::parse_exponent()
	{
		CPPGLTF_ASSERT(current_ < end_ && ('E' == *current_ || 'e' == *current_));
		++current_;
		if (end_ <= current_) {
			return -1;
		}
		if('-' == *current_ || '+' == *current_){
			++current_;
			if (end_ <= current_) {
				return -1;
			}
		}

		if(!('0'<=*current_ && *current_<='9')){
			return -1;
		}
		s32 count = 0;
		while(current_<end_){
			if(!('0'<=*current_ && *current_<='9')){
				break;
			}
			++current_;
			++count;
		}
		return count;

	}

	u32 JsonParser::get_position() const
	{
		u64 duration = std::distance(begin_, current_);
		CPPGLTF_ASSERT(duration<=0xFFFFFFFFU);
		return static_cast<u32>(duration);
	}

	JsonParser::Result JsonParser::get_number(u32 begin)
	{
		u32 index = buffer_.size();
		Value value;
		value.position_ = begin;
		value.length_ = get_position() - begin;
		value.next_ = Invalid;
		value.type_ = Type::Number;
		buffer_.push_back(value);
		return { true, index };
	}

	bool JsonParser::is_hex_digit(s32 c)
	{
		if('0'<=c && c<='9'){
			return true;
		}
		if('A'<=c && c<='F'){
			return true;
		}
		if('a'<=c && c<='f'){
			return true;
		}
		return false;
	}
} // namespace cppgltf
