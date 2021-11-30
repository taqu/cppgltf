#ifndef INC_CPPTOML_H_
#define INC_CPPTOML_H_
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
#include <cstdint>
#include <cassert>

namespace cppgltf
{
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

using std::uintptr_t;

#ifdef _DEBUG
#    define CPPGLTF_DEBUG
#endif

#ifndef CPPGLTF_NULL
#    ifdef __cplusplus
#        if 201103L <= __cplusplus || 1700 <= _MSC_VER
#            define CPPGLTF_NULL nullptr
#        else
#            define CPPGLTF_NULL 0
#        endif
#    else
#        define CPPGLTF_NULL (void*)0
#    endif
#endif

#ifdef _DEBUG
#define CPPGLTF_ASSERT(exp) assert(exp)
#else
#define CPPGLTF_ASSERT(exp)
#endif

typedef void* (*cppgltf_malloc)(size_t);
typedef void (*cppgltf_free)(void*);

static constexpr u32 DefaultMaxNest = 512;

//--- JsonParser
//---------------------------------------
class JsonParser
{
public:
    using cursor = const char*;
    static constexpr u32 Invalid = 0xFFFFFFFFU;

    enum class Type
    {
        Object,
        Array,
        String,
        Number,
        True,
        False,
        Null,
        KeyValue,
    };

    struct Result
    {
        bool success_;
        u32 index_;
    };

    struct Object
    {
        u32 size_;
    };

    struct Array
    {
        u32 size_;
    };

    struct KeyValue
    {
        u32 key_;
        u32 value_;
    };

    struct Value
    {
        u32 position_;
        u32 length_;
        u32 next_;
        Type type_;
        union {
            Object object_;
            Array array_;
            KeyValue key_value_;
        };
    };

    JsonParser(cppgltf_malloc allocator=nullptr, cppgltf_free deallocator=nullptr, u32 max_nests = DefaultMaxNest);
    ~JsonParser();

    bool parse(cursor head, cursor end);

    u32 size() const;
    const Value& getRoot() const;
private:
    JsonParser(const JsonParser&) = delete;
    JsonParser& operator=(const JsonParser&) = delete;

    struct Buffer
    {
    public:
        Buffer(cppgltf_malloc allocator=nullptr, cppgltf_free deallocator=nullptr);
        ~Buffer();

        u32 capacity() const;
        u32 size() const;
        void clear();
        void push_back(const Value& value);

        const Value& operator[](u32 index) const;
        Value& operator[](u32 index);
    private:
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;
		cppgltf_malloc allocator_;
		cppgltf_free deallocator_;
        u32 capacity_;
        u32 size_;
        Value* values_;
    };
    
    void add_element_to_object(u32 object_index, u32 element_index);
    void add_element_to_array(u32 array_index, u32 element_index);

    void skip_bom();
    void skip_space();
    Result parse_value();
    Result parse_object();
    Result parse_array();
    Result parse_string();
    Result parse_true();
    Result parse_false();
    Result parse_null();
    Result parse_number();

    s32 parse_escape();
    s32 parse_utf8();
    s32 parse_4hexdigit();
    void parse_integer();
    s32 parse_fraction();
    s32 parse_exponent();
    u32 get_position() const;
    Result get_number(u32 begin);
    static bool is_hex_digit(s32 c);

    cppgltf_malloc allocator_;
    cppgltf_free deallocator_;
    u32 max_nests_;
    u32 nest_count_;
    cursor begin_ = nullptr;
    cursor current_ = nullptr;
    cursor end_ = nullptr;
    Buffer buffer_;
};

//--- GLTFParser
//---------------------------------------
class GLTFParser
{
public:
    using cursor = const char*;

    GLTFParser(cppgltf_malloc allocator=nullptr, cppgltf_free deallocator=nullptr, u32 max_nests = DefaultMaxNest);
    ~GLTFParser();

    bool parse(cursor head, cursor end);

private:
    GLTFParser(const GLTFParser&) = delete;
    GLTFParser& operator=(const GLTFParser&) = delete;
};

} // namespace cppgltf
#endif //INC_CPPTOML_H_

