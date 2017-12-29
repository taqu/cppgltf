#ifndef INC_CPPGLTF_H_
#define INC_CPPGLTF_H_
/*
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
/**
@author t-sakai
@date 2017/09/06 create

USAGE:
put '#define CPPGLTF_IMPLEMENTATION' before including this file to create the implementation.
*/
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <cstdarg>
#include <cfloat>
#include <cmath>

namespace cppgltf
{
#ifdef __cplusplus
#   if 201103L<=__cplusplus || 1900<=_MSC_VER
#       define CPPGLTF_CPP11 1
#   endif
#endif

#ifdef __cplusplus
#   ifdef CPPGLTF_CPP11
#       define CPPGLTF_NULL nullptr
#   else
#       define CPPGLTF_NULL 0
#   endif
#else
#   define CPPGLTF_NULL (void*)0
#endif

#ifndef CPPGLTF_TYPES
#define CPPGLTF_TYPES
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

    typedef char Char;
    typedef bool boolean;

    typedef std::size_t size_type;

    using std::move;

#ifdef _MSC_VER
#ifndef CPPGLTF_OFF_T
#define CPPGLTF_OFF_T
    typedef s32 off_t;
#endif

    #ifndef CPPGLTF_FSEEK
    #define CPPGLTF_FSEEK(f,p,o) fseek((f),(p),(o))
    #endif

    #ifndef CPPGLTF_FTELL
    #define CPPGLTF_FTELL(f) ftell((f))
    #endif

#else
#ifndef CPPGLTF_OFF_T
#define CPPGLTF_OFF_T
    typedef s32 off_t;
#endif

    #ifndef CPPGLTF_FSEEK
    #define CPPGLTF_FSEEK(f,p,o) fseek((f),(p),(o))
    #endif

    #ifndef CPPGLTF_FTELL
    #define CPPGLTF_FTELL(f) ftell((f))
    #endif
#endif

#endif//CPPGLTF_TYPES

#ifndef CPPGLTF_MALLOC
#define CPPGLTF_MALLOC(size) malloc(size)
#endif

#ifndef CPPGLTF_FREE
#define CPPGLTF_FREE(ptr) free(ptr)
#endif

#ifndef CPPGLTF_PLACEMENT_NEW
#define CPPGLTF_PLACEMENT_NEW(ptr) new(ptr)
#endif

#ifndef CPPGLTF_ASSERT
#define CPPGLTF_ASSERT(exp) assert(exp)
#endif

    template<class T>
    inline void swap(T& l, T& r)
    {
        T tmp(move(l));
        l = move(r);
        r = move(tmp);
    }

    template<class T>
    inline T maximum(const T& x0, const T& x1)
    {
        return x0<x1? x1:x0;
    }

    template<class T>
    inline T minimum(const T& x0, const T& x1)
    {
        return x0<x1? x0:x1;
    }

    union UnionU32F32
    {
        u32 u32_;
        f32 f32_;
    };
    inline f32 absolute(f32 val)
    {
        UnionU32F32 u;
        u.f32_ = val;
        u.u32_ &= 0x7FFFFFFFU;
        return u.f32_;
    }

    inline bool equal(f32 x0, f32 x1)
    {
        return absolute(x0-x1)<=FLT_EPSILON;
    }

    boolean isBase64(s32 c);
    s32 getLengthEncodedBase64(s32 l);
    s32 getLengthDecodedBase64(s32 l);
    s32 decodeBase64(u8* dst, FILE* file);
    /**
    @return decoded byte count
    */
    s32 decodeBase64(u8* dst, s32 length, const s8* src);
    s32 encodeBase64(FILE* file, s32 length, const u8* src);
    /**
    @return encoded char count
    */
    s32 encodeBase64(s8* dst, s32 length, const u8* src);

    f32 toFloat(s8 x);
    f32 toFloat(u8 x);
    f32 toFloat(s16 x);
    f32 toFloat(u16 x);

    s8 toS8(f32 x);
    u8 toU8(f32 x);
    s16 toS16(f32 x);
    u16 toU16(f32 x);

    //--------------------------------------------
    //---
    //--- String
    //---
    //--------------------------------------------
    class String
    {
    public:
        static const s32 ExpandSize = 8;
        static const u32 ExpandMask = ExpandSize-1;

        String();
        String(String&& rhs);
        ~String();

        void clear();
        void resize(s32 length);
        void reserve(s32 capacity);

        inline s32 capacity() const;
        inline s32 length() const;
        inline Char* c_str();
        inline const Char* c_str() const;
        inline const Char& operator[](s32 index) const;
        inline Char& operator[](s32 index);

        String& operator=(String&& rhs);
        String& operator=(const Char* str);
        inline void assign(const Char* str);
        void assign(s32 length, const Char* str);
        void push_back(Char c);

        boolean operator==(const String& rhs) const;
        boolean startWith(const Char* str) const;

        friend boolean operator==(const String& lhs, const Char* rhs);
        friend boolean operator==(const Char* lhs, const String& rhs);
    private:
        static inline s32 getCapacity(s32 capacity)
        {
            return (capacity+ExpandMask) & ~ExpandMask;
        }

        inline const Char* getBuffer() const
        {
            return (capacity_<=ExpandSize)? buffer_.small_ : buffer_.elements_;
        }
        inline Char* getBuffer()
        {
            return (capacity_<=ExpandSize)? buffer_.small_ : buffer_.elements_;
        }


        void initBuffer(s32 length);
        void createBuffer(s32 length);
        void expandBuffer(s32 length);

        s32 capacity_;
        s32 length_;

        union Buffer
        {
            Char small_[ExpandSize];
            Char* elements_;
        };
        Buffer buffer_;
    };

    inline s32 String::capacity() const
    {
        return capacity_;
    }

    inline s32 String::length() const
    {
        return length_;
    }

    inline Char* String::c_str()
    {
        return (ExpandSize<capacity_)? buffer_.elements_ : buffer_.small_;
    }

    inline const Char* String::c_str() const
    {
        return (ExpandSize<capacity_)? buffer_.elements_ : buffer_.small_;
    }

    inline const Char& String::operator[](s32 index) const
    {
        CPPGLTF_ASSERT(0<=index && index<length_);
        return getBuffer()[index];
    }

    inline Char& String::operator[](s32 index)
    {
        CPPGLTF_ASSERT(0<=index && index<length_);
        return getBuffer()[index];
    }

    inline void String::assign(const Char* str)
    {
        assign(static_cast<s32>(::strlen(str)), str);
    }

    inline s32 compare(const String& lhs, const String& rhs)
    {
        return ::strcmp(lhs.c_str(), rhs.c_str());
    }

    inline s32 compare(const String& lhs, const Char* rhs)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != rhs);
        return ::strcmp(lhs.c_str(), rhs);
    }

    inline s32 compare(const Char* lhs, const String& rhs)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != lhs);
        return ::strcmp(lhs, rhs.c_str());
    }

    //--------------------------------------------
    //---
    //--- Array
    //---
    //--------------------------------------------
    template<class T>
    class Array
    {
    public:
        typedef Array<T> this_type;
        typedef s32 size_type;
        typedef T* iterator;
        typedef const T* const_iterator;

        Array();
        Array(this_type&& rhs);
        explicit Array(s32 capacity);
        ~Array();

        inline s32 size() const;
        inline s32 capacity() const;

        inline T& operator[](s32 index);
        inline const T& operator[](s32 index) const;
        inline T& front();
        inline const T& front() const;
        inline T& back();
        inline const T& back() const;

        void push_back(const T& t);
        void push_back(T&& t);
        void pop_back();

        void clear();
        void reserve(s32 capacity);
        void resize(s32 size);
        void removeAt(s32 index);
        void swap(this_type& rhs);

        this_type& operator=(this_type&& rhs);
    private:
        Array(const this_type&) = delete;
        this_type& operator=(const this_type&) = delete;

        s32 capacity_;
        s32 size_;
        T *items_;
    };

    template<class T>
    Array<T>::Array()
        :capacity_(0)
        ,size_(0)
        ,items_(CPPGLTF_NULL)
    {
    }

    template<class T>
    Array<T>::Array(this_type&& rhs)
        :capacity_(rhs.capacity_)
        ,size_(rhs.size_)
        ,items_(rhs.items_)
    {
        rhs.capacity_ = 0;
        rhs.size_ = 0;
        rhs.items_ = CPPGLTF_NULL;
    }

    template<class T>
    Array<T>::Array(s32 capacity)
        :capacity_(capacity)
        ,size_(0)
        ,items_(CPPGLTF_NULL)
    {
        CPPGLTF_ASSERT(0<=capacity_);
        items_ = (T*)CPPGLTF_MALLOC(sizeof(T)*capacity_);
    }

    template<class T>
    Array<T>::~Array()
    {
        for(s32 i=0; i<size_; ++i){
            items_[i].~T();
        }
        CPPGLTF_FREE(items_);
        items_ = CPPGLTF_NULL;
    }

    template<class T>
    inline s32 Array<T>::size() const
    {
        return size_;
    }

    template<class T>
    inline s32 Array<T>::capacity() const
    {
        return capacity_;
    }

    template<class T>
    inline T& Array<T>::operator[](s32 index)
    {
        CPPGLTF_ASSERT(0<=index && index<size_);
        return items_[index];
    }

    template<class T>
    inline const T& Array<T>::operator[](s32 index) const
    {
        CPPGLTF_ASSERT(0<=index && index<size_);
        return items_[index];
    }

    template<class T>
    inline T& Array<T>::front()
    {
        CPPGLTF_ASSERT(0<size_);
        return items_[0];
    }

    template<class T>
    inline const T& Array<T>::front() const
    {
        CPPGLTF_ASSERT(0<size_);
        return items_[0];
    }

    template<class T>
    inline T& Array<T>::back()
    {
        CPPGLTF_ASSERT(0<size_);
        return items_[size_-1];
    }

    template<class T>
    inline const T& Array<T>::back() const
    {
        CPPGLTF_ASSERT(0<size_);
        return items_[size_-1];
    }


    template<class T>
    void Array<T>::push_back(const T& t)
    {
        if(capacity_<=size_){
            reserve(capacity_+16);
        }
        CPPGLTF_PLACEMENT_NEW(&items_[size_]) T(t);
        ++size_;
    }


    template<class T>
    void Array<T>::push_back(T&& t)
    {
        if(capacity_<=size_){
            reserve(capacity_+16);
        }
        CPPGLTF_PLACEMENT_NEW(&items_[size_]) T(t);
        ++size_;
    }

    template<class T>
    void Array<T>::pop_back()
    {
        CPPGLTF_ASSERT(0<size_);
        --size_;
        items_[size_].~T();
    }

    template<class T>
    void Array<T>::clear()
    {
        for(s32 i=0; i<size_; ++i){
            items_[i].~T();
        }
        size_ = 0;
    }

    template<class T>
    void Array<T>::reserve(s32 capacity)
    {
        if(capacity<=capacity_){
            return;
        }

        T *newItems = (T*)CPPGLTF_MALLOC(capacity*sizeof(T));

        for(s32 i=0; i<size_; ++i){
            CPPGLTF_PLACEMENT_NEW(&newItems[i]) T(std::move(items_[i]));
            items_[i].~T();
        }
        CPPGLTF_FREE(items_);
        items_ = newItems;
        capacity_ = capacity;
    }

    template<class T>
    void Array<T>::resize(s32 size)
    {
        if(size < size_){
            for(s32 i=size; i<size_; ++i){
                items_[i].~T();
            }

        }else{
            reserve(size);
            for(s32 i=size_; i<size; ++i){
                CPPGLTF_PLACEMENT_NEW(&items_[i]) T;
            }
        }
        size_ = size;
    }

    template<class T>
    void Array<T>::removeAt(s32 index)
    {
        CPPGLTF_ASSERT(0<=index && index<size_);
        for(s32 i=index+1; i<size_; ++i){
            items_[i-1] = std::move(items_[i]);
        }
        --size_;
        items_[size_].~T();
    }

    template<class T>
    void Array<T>::swap(this_type& rhs)
    {
        swap(capacity_, rhs.capacity_);
        swap(size_, rhs.size_);
        swap(items_, rhs.items_);
    }

    template<class T>
    typename Array<T>::this_type& Array<T>::operator=(this_type&& rhs)
    {
        if(this == &rhs){
            return *this;
        }
        capacity_ = rhs.capacity_;
        size_ = rhs.size_;
        items_ = rhs.items_;

        rhs.capacity_ = 0;
        rhs.size_ = 0;
        rhs.items_ = CPPGLTF_NULL;
        return *this;
    }

    //----------------------------------------------------
    //---
    //--- FStream
    //---
    //----------------------------------------------------
    template<class T>
    class FStream : public T
    {
    public:
        void close();

        virtual bool valid() const;
        virtual bool seek(off_t pos, s32 whence);
        virtual off_t tell();

    protected:
        FStream(const FStream&) = delete;
        FStream& operator=(const FStream&) = delete;

        bool open(const Char* filepath, const Char* mode);

        FStream();
        FStream(FStream&& rhs);
        ~FStream();

        FILE* file_;
    };

    template<class T>
    FStream<T>::FStream()
        :file_(CPPGLTF_NULL)
    {}

    template<class T>
    FStream<T>::FStream(FStream&& rhs)
        :file_(rhs.file_)
    {
        rhs.file_ = CPPGLTF_NULL;
    }

    template<class T>
    FStream<T>::~FStream()
    {
        close();
    }

    template<class T>
    bool FStream<T>::open(const Char* filepath, const Char* mode)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != filepath);
        CPPGLTF_ASSERT(CPPGLTF_NULL != mode);
#ifdef _MSC_VER
        FILE* file;
        if(0 != fopen_s(&file, filepath, mode)){
            return false;
        }
#else
        FILE* file = fopen(filepath, mode);
        if(CPPGLTF_NULL == file){
            return false;
        }
#endif
        close();
        file_ = file;
        return true;
    }

    template<class T>
    void FStream<T>::close()
    {
        if(CPPGLTF_NULL != file_){
            fclose(file_);
            file_ = CPPGLTF_NULL;
        }
    }

    template<class T>
    bool FStream<T>::valid() const
    {
        return CPPGLTF_NULL != file_;
    }

    template<class T>
    bool FStream<T>::seek(off_t pos, s32 whence)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != file_);
        return 0<=CPPGLTF_FSEEK(file_, pos, whence);
    }

    template<class T>
    off_t FStream<T>::tell()
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != file_);
        return CPPGLTF_FTELL(file_);
    }

    //---------------------------------------------------------------
    //---
    //--- IStream
    //---
    //---------------------------------------------------------------
    class IStream
    {
    public:
        /**
        @return Success: position, Faile: -1
        */
        virtual off_t tell() const =0;
        virtual boolean seek(off_t pos) =0;

        /**
        @return Success: input c, Fail: EOF
        */
        virtual s32 get() =0;

        /**
        @return Success: input c, Fail: EOF
        */
        virtual s32 unget(s32 c) =0;
        virtual s32 read(u8* dst, s32 size) =0;

        virtual boolean isEOF() const =0;

        template<class T>
        s32 read(T& t)
        {
            return read(reinterpret_cast<u8*>(&t), sizeof(T));
        }
    protected:
        IStream(const IStream&) = delete;
        IStream& operator=(const IStream&) = delete;

        IStream()
        {}
        virtual ~IStream()
        {}
    };

    class ISStream : public IStream
    {
    public:
        ISStream();
        ISStream(ISStream&& rhs);
        ISStream(off_t length, const u8* str);
        virtual ~ISStream();

        off_t tell() const override;
        boolean seek(off_t pos) override;
        s32 get() override;
        s32 unget(s32 c) override;
        s32 read(u8* dst, s32 size) override;

        boolean isEOF() const override;

        ISStream& operator=(ISStream&& rhs);
    protected:
        ISStream(const ISStream&) = delete;
        ISStream& operator=(const ISStream&) = delete;

        off_t pos_;
        off_t length_;
        const u8* str_;
    };

    class IFStream : public FStream<IStream>
    {
    public:
        typedef FStream<IStream> parent_type;

        IFStream();
        IFStream(IFStream&& rhs);
        virtual ~IFStream();

        bool open(const Char* filepath);
        off_t tell() const override;
        boolean seek(off_t pos) override;
        s32 get() override;
        s32 unget(s32 c) override;
        s32 read(u8* dst, s32 size) override;

        boolean isEOF() const override;

        IFStream& operator=(IFStream&& rhs);
    protected:
        IFStream(const IFStream&) = delete;
        IFStream& operator=(const IFStream&) = delete;

        off_t pos_;
    };


    //---------------------------------------------------------------
    //---
    //--- OStream
    //---
    //---------------------------------------------------------------
    class OStream
    {
    public:
        s32 write(const Char* src)
        {
            return write(static_cast<s32>(sizeof(Char)*strlen(src)), reinterpret_cast<const u8*>(src));
        }
        s32 write(s32 size, const Char* src)
        {
            return write(sizeof(Char)*size, reinterpret_cast<const u8*>(src));
        }

        virtual s32 write(s32 size, const u8* src) =0;
        template<class T>
        s32 write(const T& t)
        {
            return write(sizeof(T), reinterpret_cast<const u8*>(&t));
        }

        virtual boolean replaceLast(s32 backLen, const Char* str) =0;
    protected:
        OStream(const OStream&) = delete;
        OStream& operator=(const OStream&) = delete;

        OStream()
        {}
        virtual ~OStream()
        {}
    };

    class OSStream : public OStream
    {
    public:
        OSStream();
        OSStream(s32 capacity);
        OSStream(OSStream&& rhs);
        virtual ~OSStream();

        s32 size() const;
        const u8* buff() const;

        s32 write(s32 size, const u8* src) override;
        boolean replaceLast(s32 backLen, const Char* str) override;

        OSStream& operator=(OSStream&& rhs);
    protected:
        OSStream(const OSStream&) = delete;
        OSStream& operator=(const OSStream&) = delete;

        static s32 calcNext(s32 request, s32 capacity);

        s32 capacity_;
        s32 pos_;
        u8* buffer_;
    };

    class OFStream : public FStream<OStream>
    {
    public:
        typedef FStream<OStream> parent_type;

        OFStream();
        OFStream(OFStream&& rhs);
        virtual ~OFStream();

        bool open(const Char* filepath);
        s32 write(s32 size, const u8* src) override;
        boolean replaceLast(s32 backLen, const Char* str) override;

        OFStream& operator=(OFStream&& rhs);
    protected:
        OFStream(const OFStream&) = delete;
        OFStream& operator=(const OFStream&) = delete;
    };

    //---------------------------------------------------------------
    enum JSONStatus
    {
        JSON_Error = -1,
        JSON_OK =0,
    };
    enum JSONToken
    {
        JSONToken_Error = -1,
        JSONToken_None = 0,
        JSONToken_OBJStart = '{',
        JSONToken_OBJEnd = '}',
        JSONToken_OBJColon = ':',
        JSONToken_Separator = ',',
        JSONToken_ArrayStart = '[',
        JSONToken_ArrayEnd = ']',
        JSONToken_DQuote = '"',
        JSONToken_Escape = '\\',
        JSONToken_Value = 0x80,
    };

    enum JSON
    {
        JSON_Object =1,
        JSON_Array,
        JSON_String,
        JSON_Integer,
        JSON_Float,
        JSON_True,
        JSON_False,
        JSON_Null,
    };

    //---------------------------------------------------------------
    //---
    //--- Range
    //---
    //---------------------------------------------------------------
    struct Range
    {
        off_t start_;
        off_t length_;

        inline void reset()
        {
            start_ = length_ = 0;
        }
    };

    //---------------------------------------------------------------
    //---
    //--- RangeStream
    //---
    //---------------------------------------------------------------
    class RangeStream
    {
    public:
        RangeStream(IStream* stream, const Range& range);
        off_t length() const;
        s32 read(u8* dst);
        s32 readAsString(Char* str);
        s32 readAsString(String& str);
        s32 readAsInt(s32 defaultValue=0);
        f32 readAsFloat(f32 defaultValue=0.0f);
    private:
        IStream* stream_;
        off_t start_;
        off_t length_;
    };

    //---------------------------------------------------------------
    //---
    //--- JSONEventHandler
    //---
    //---------------------------------------------------------------
    class JSONEventHandler
    {
    public:
        virtual void begin() =0;
        virtual void end() =0;
        virtual void beginObject() =0;
        virtual void endObject(RangeStream object) =0;

        virtual void beginArray() =0;
        virtual void endArray(RangeStream stream) =0;

        virtual void root(s32 type, RangeStream value) =0;
        virtual void value(s32 type, RangeStream v) =0;
        virtual void keyValue(RangeStream key, s32 type, RangeStream value) =0;

        virtual void onError(s32 line, s32 charCount) =0;
    protected:
        JSONEventHandler(const JSONEventHandler&) = delete;
        JSONEventHandler& operator=(const JSONEventHandler&) = delete;

        JSONEventHandler()
        {}
        virtual ~JSONEventHandler()
        {}
    };

    //---------------------------------------------------------------
    //---
    //--- JSONReader
    //---
    //---------------------------------------------------------------
    class JSONReader
    {
    public:
        static const s32 Flag_ErrorReported = (0x01<<0);

        JSONReader(IStream& istream, JSONEventHandler& handler);
        ~JSONReader();

        boolean read(s32 flags = 0);
    protected:
        boolean skipSpace();
        boolean check(const s8* str);
        void setStart(Range& range);
        void setLength(Range& range);
        s32 onError();

        s32 getObject(Range& object);
        s32 getArray(Range& array);
        s32 getValue(Range& value);
        s32 getString(Range& str);
        /**
        @return Success:JSON_Integer of JSON_Float, Fail:JSON_Error
        */
        s32 getNumber(Range& number);
        s32 getDigit(boolean unget=true);
        boolean isNumber(s32 c) const;
        boolean isOneToNine(s32 c) const;

        s32 flags_;
        IStream& istream_;
        JSONEventHandler& handler_;
    };

    //---------------------------------------------------------------
    //---
    //--- glTF JSON
    //---
    //---------------------------------------------------------------
    static const s32 GLTF_BUFFERVIEW = 0;
    static const s32 GLTF_BYTEOFFSET = 1;
    static const s32 GLTF_COMPONENTTYPE = 2;
    static const s32 GLTF_NORMALIZED = 3;
    static const s32 GLTF_COUNT = 4;
    static const s32 GLTF_TYPE = 5;
    static const s32 GLTF_MAX = 6;
    static const s32 GLTF_MIN = 7;
    static const s32 GLTF_SPARSE = 8;
    static const s32 GLTF_NAME = 9;
    static const s32 GLTF_EXTENSIONS = 10;
    static const s32 GLTF_EXTRAS = 11;
    static const s32 GLTF_CHANNELS = 12;
    static const s32 GLTF_SAMPLERS = 13;
    static const s32 GLTF_INPUT = 14;
    static const s32 GLTF_INTERPOLATION = 15;
    static const s32 GLTF_OUTPUT = 16;
    static const s32 GLTF_COPYRIGHT = 17;
    static const s32 GLTF_GENERATOR = 18;
    static const s32 GLTF_VERSION = 19;
    static const s32 GLTF_MINVERSION = 20;
    static const s32 GLTF_URI = 21;
    static const s32 GLTF_BYTELENGTH = 22;
    static const s32 GLTF_BUFFER = 23;
    static const s32 GLTF_BYTESTRIDE = 24;
    static const s32 GLTF_TARGET = 25;
    static const s32 GLTF_ORTHOGRAPHIC = 26;
    static const s32 GLTF_PERSPECTIVE = 27;
    static const s32 GLTF_SAMPLER = 28;
    static const s32 GLTF_EXTENSIONSUSED = 29;
    static const s32 GLTF_EXTENSIONSREQUIRED = 30;
    static const s32 GLTF_ACCESSORS = 31;
    static const s32 GLTF_ANIMATIONS = 32;
    static const s32 GLTF_ASSET = 33;
    static const s32 GLTF_BUFFERS = 34;
    static const s32 GLTF_BUFFERVIEWS = 35;
    static const s32 GLTF_CAMERAS = 36;
    static const s32 GLTF_IMAGES = 37;
    static const s32 GLTF_MATERIALS = 38;
    static const s32 GLTF_MESHES = 39;
    static const s32 GLTF_NODES = 40;
    static const s32 GLTF_SCENE = 41;
    static const s32 GLTF_SCENES = 42;
    static const s32 GLTF_SKINS = 43;
    static const s32 GLTF_TEXTURES = 44;
    static const s32 GLTF_MIMETYPE = 45;
    static const s32 GLTF_PBRMETALLICROUGHNESS = 46;
    static const s32 GLTF_NORMALTEXTURE = 47;
    static const s32 GLTF_OCCLUSIONTEXTURE = 48;
    static const s32 GLTF_EMISSIVETEXTURE = 49;
    static const s32 GLTF_EMISSIVEFACTOR = 50;
    static const s32 GLTF_ALPHAMODE = 51;
    static const s32 GLTF_ALPHACUTOFF = 52;
    static const s32 GLTF_DOUBLESIDED = 53;
    static const s32 GLTF_PRIMITIVES = 54;
    static const s32 GLTF_WEIGHTS = 55;
    static const s32 GLTF_CAMERA = 56;
    static const s32 GLTF_CHILDREN = 57;
    static const s32 GLTF_SKIN = 58;
    static const s32 GLTF_MATRIX = 59;
    static const s32 GLTF_MESH = 60;
    static const s32 GLTF_ROTATION = 61;
    static const s32 GLTF_SCALE = 62;
    static const s32 GLTF_TRANSLATION = 63;
    static const s32 GLTF_INDEX = 64;
    static const s32 GLTF_TEXCOORD = 65;
    static const s32 GLTF_STRENGTH = 66;
    static const s32 GLTF_XMAG = 67;
    static const s32 GLTF_YMAG = 68;
    static const s32 GLTF_ZFAR = 69;
    static const s32 GLTF_ZNEAR = 70;
    static const s32 GLTF_BASECOLORFACTOR = 71;
    static const s32 GLTF_BASECOLORTEXTURE = 72;
    static const s32 GLTF_METALLICFACTOR = 73;
    static const s32 GLTF_ROUGHNESSFACTOR = 74;
    static const s32 GLTF_METALLICROUGHNESSTEXTURE = 75;
    static const s32 GLTF_ASPECTRATIO = 76;
    static const s32 GLTF_YFOV = 77;
    static const s32 GLTF_ATTRIBUTES = 78;
    static const s32 GLTF_INDICES = 79;
    static const s32 GLTF_MATERIAL = 80;
    static const s32 GLTF_MODE = 81;
    static const s32 GLTF_TARGETS = 82;
    static const s32 GLTF_MAGFILTER = 83;
    static const s32 GLTF_MINFILTER = 84;
    static const s32 GLTF_WRAPS = 85;
    static const s32 GLTF_WRAPT = 86;
    static const s32 GLTF_INVERSEBINDMATRICES = 87;
    static const s32 GLTF_SKELETON = 88;
    static const s32 GLTF_JOINTS = 89;
    static const s32 GLTF_VALUES = 90;
    static const s32 GLTF_NODE = 91;
    static const s32 GLTF_PATH = 92;
    static const s32 GLTF_SOURCE = 93;
    static const s32 GLTF_POSITION = 94;
    static const s32 GLTF_NORMAL = 95;
    static const s32 GLTF_TANGENT = 96;
    static const s32 GLTF_TEXCOORD_0 = 97;
    static const s32 GLTF_TEXCOORD_1 = 98;
    static const s32 GLTF_TEXCOORD_2 = 99;
    static const s32 GLTF_TEXCOORD_3 = 100;
    static const s32 GLTF_COLOR_0 = 101;
    static const s32 GLTF_COLOR_1 = 102;
    static const s32 GLTF_COLOR_2 = 103;
    static const s32 GLTF_COLOR_3 = 104;
    static const s32 GLTF_JOINTS_0 = 105;
    static const s32 GLTF_JOINTS_1 = 106;
    static const s32 GLTF_JOINTS_2 = 107;
    static const s32 GLTF_JOINTS_3 = 108;
    static const s32 GLTF_WEIGHTS_0 = 109;
    static const s32 GLTF_WEIGHTS_1 = 110;
    static const s32 GLTF_WEIGHTS_2 = 111;
    static const s32 GLTF_WEIGHTS_3 = 112;

    struct JSKeyValue;
    class Allocator;

    struct JSAny
    {
        s32 type_;
        s32 num_;
        union
        {
            s32 int_;
            f32 float_;
            s32 string_;
            JSAny* array_;
            JSKeyValue* object_;
        };

        static JSAny createVector(Allocator& allocator, s32 num, s32 type);
        static void destroy(Allocator& allocator, JSAny& any);
    };

    struct JSKeyValue
    {
        s32 key_;
        JSAny value_;

        inline static void destroy(Allocator& allocator, JSKeyValue& keyValue)
        {
            JSAny::destroy(allocator, keyValue.value_);
        }
    };

    struct JSElement
    {
        s32 capacity_;
        JSAny value_;

        static void destroy(Allocator& allocator, JSElement& element);
    };

    class Allocator
    {
    public:
        struct Page
        {
            Page* next_;
            s32 capacity_;
        };

        union PageMem
        {
            JSKeyValue value_;
            Page page_;
        };

        static const s32 UnitSize = sizeof(PageMem);
        static const s32 PageSize = 4069;
        static const s32 UnitNum = PageSize/UnitSize;
        static const s32 TableSize = 10;
        static const s32 MaxNum = 160;

        static const s32 SmaleSizes[];

        Allocator();
        ~Allocator();

        void destroy();
        void* alloc(s32 num);
        void dealloc(s32 num, void* ptr);
    private:
        Allocator(const Allocator&) = delete;
        Allocator& operator=(const Allocator&) = delete;

        void* allocate(s32 num);

        Page* table_[TableSize]; //1,2,4,8,16,32,64,96,128,160
        Page* large_;
        Page* top_;
        Page* largeTop_;
    };

    //---------------------------------------------------------------
    //---
    //--- glTFBase
    //---
    //---------------------------------------------------------------
    union Number
    {
        s32 ivalue_;
        u32 uivalue_;
        f32 fvalue_;
    };

    class glTFBase : public JSONEventHandler
    {
    public:
        static const s32 InitSize = 2;

        glTFBase();
        virtual ~glTFBase();

        void destroy();

        void begin() override;
        void end() override;

        void beginObject() override;
        void endObject(RangeStream object) override;
        void beginArray() override;
        void endArray(RangeStream stream) override;
        void root(s32 type, RangeStream value) override;
        void value(s32 type, RangeStream v) override;
        void keyValue(RangeStream key, s32 type, RangeStream value) override;

        void onError(s32 line, s32 charCount) override;

    protected:
        virtual void parseGlTF(const JSAny& glTF) =0;

        void expand();
        void expandObject(JSElement& element);
        void expandArray(JSElement& element);

        void getString(String& str, const JSAny& value) const;
        bool getBoolean(const JSAny& value) const;
        f32 getNumber(const JSAny& value) const;
        void getNumbers(s32 num, Number* x, const JSAny& value) const;
        void getVec2(f32 v[2], const JSAny& value) const;
        void getVec3(f32 v[3], const JSAny& value) const;
        void getVec4(f32 v[4], const JSAny& value) const;
        void getMat44(f32 m[16], const JSAny& value) const;

        template<class T>
        inline void expand(Array<T>& array, const JSAny& value)
        {
            CPPGLTF_ASSERT(JSON_Array == value.type_);
            if(array.size()<=value.num_){
                array.resize(value.num_);
            }
        }
        template<class T>
        inline void expandObjects(Array<T>& array, const JSAny& value)
        {
            CPPGLTF_ASSERT(JSON_Object == value.type_);
            if(array.size()<=value.num_){
                array.resize(value.num_);
            }
        }

        static s32 getKeyCode(const String& key);

        class StringTable
        {
        public:
            StringTable();
            ~StringTable();

            void clear();
            s32 add(s32 length);

            const Char* c_str(s32 start) const;
            Char* c_str(s32 start);
        private:
            s32 capacity_;
            s32 length_;
            Char* strings_;
        };

        Allocator allocator_;

        s32 size_;
        s32 capacity_;
        JSElement* elements_;

        String key_;
        StringTable strings_;
    };

    //---------------------------------------------------------------
    //---
    //--- glTF
    //---
    //---------------------------------------------------------------
    static const s32 GLTF_TYPE_BYTE = 5120;
    static const s32 GLTF_TYPE_UNSIGNED_BYTE = 5121;
    static const s32 GLTF_TYPE_SHORT = 5122;
    static const s32 GLTF_TYPE_UNSIGNED_SHORT = 5123;
    static const s32 GLTF_TYPE_INT = 5124;
    static const s32 GLTF_TYPE_UNSIGNED_INT = 5125;
    static const s32 GLTF_TYPE_FLOAT = 5126;

    static const s32 GLTF_NEAREST = 9728;
    static const s32 GLTF_LINEAR = 9729;
    static const s32 GLTF_NEAREST_MIPMAP_NEAREST = 9984;
    static const s32 GLTF_LINEAR_MIPMAP_NEAREST = 9985;
    static const s32 GLTF_NEAREST_MIPMAP_LINEAR = 9986;
    static const s32 GLTF_LINEAR_MIPMAP_LINEAR = 9987;

    static const s32 GLTF_INTERP_LINEAR = 0;
    static const s32 GLTF_INTERP_STEP = 1;
    static const s32 GLTF_INTERP_CATMULLROMSPLINE = 2;
    static const s32 GLTF_INTERP_CUBICSPLINE = 3;
#define GLTF_INTERP_NAME_LINEAR "LINEAR"
#define GLTF_INTERP_NAME_STEP "STEP"
#define GLTF_INTERP_NAME_CATMULLROMSPLINE "CATMULLROMSPLINE"
#define GLTF_INTERP_NAME_CUBICSPLINE "CUBICSPLINE"

    static const s32 GLTF_REPEAT = 10497;
    static const s32 GLTF_CLAMP_TO_EDGE = 33071;
    static const s32 GLTF_MIRRORED_REPEAT = 33648;

    static const s32 GLTF_ALPHAMODE_OPAQUE = 0;
    static const s32 GLTF_ALPHAMODE_MASK = 1;
    static const s32 GLTF_ALPHAMODE_BLEND = 2;

#define GLTF_ALPHAMODE_NAME_OPAQUE "OPAQUE"
#define GLTF_ALPHAMODE_NAME_MASK "MASK"
#define GLTF_ALPHAMODE_NAME_BLEND "BLEND"

    static const s32 GLTF_TYPE_SCALAR = 0;
    static const s32 GLTF_TYPE_VEC2 = 1;
    static const s32 GLTF_TYPE_VEC3 = 2;
    static const s32 GLTF_TYPE_VEC4 = 3;
    static const s32 GLTF_TYPE_MAT2 = 4;
    static const s32 GLTF_TYPE_MAT3 = 5;
    static const s32 GLTF_TYPE_MAT4 = 6;
#define GLTF_TYPE_NAME_SCALAR "SCALAR"
#define GLTF_TYPE_NAME_VEC2 "VEC2"
#define GLTF_TYPE_NAME_VEC3 "VEC3"
#define GLTF_TYPE_NAME_VEC4 "VEC4"
#define GLTF_TYPE_NAME_MAT2 "MAT2"
#define GLTF_TYPE_NAME_MAT3 "MAT3"
#define GLTF_TYPE_NAME_MAT4 "MAT4"

    static const s32 GLTF_ATTRIBUTE_POSITION = 0;
    static const s32 GLTF_ATTRIBUTE_NORMAL = 1;
    static const s32 GLTF_ATTRIBUTE_TANGENT = 2;
    static const s32 GLTF_ATTRIBUTE_TEXCOORD = 3;
    static const s32 GLTF_ATTRIBUTE_COLOR = 4;
    static const s32 GLTF_ATTRIBUTE_JOINTS = 5;
    static const s32 GLTF_ATTRIBUTE_WEIGHTS = 6;

    static const s32 GLTF_PRIMITIVE_POINTS = 0;
    static const s32 GLTF_PRIMITIVE_LINES = 1;
    static const s32 GLTF_PRIMITIVE_LINE_LOOP = 2;
    static const s32 GLTF_PRIMITIVE_LINE_STRIP = 3;
    static const s32 GLTF_PRIMITIVE_TRIANGLES = 4;
    static const s32 GLTF_PRIMITIVE_TRIANGLE_STRIP = 5;
    static const s32 GLTF_PRIMITIVE_TRIANGLE_FAN = 6;

    static const s32 GLTF_CAMERA_TYPE_PERSPECTIVE = 0;
    static const s32 GLTF_CAMERA_TYPE_ORTHOGRAPHIC = 1;
#define GLTF_CAMERA_TYPE_NAME_PERSPECTIVE "perspective"
#define GLTF_CAMERA_TYPE_NAME_ORTHOGRAPHIC "orthographic"

    enum GLTF_FILE
    {
        GLTF_FILE_AsIs =0,
        //GLTF_FILE_Embedded,
        //GLTF_FILE_File,
        GLTF_FILE_GLB,
    };

    static const s32 GLB_ALIGNMENT = 4;

    class Flags
    {
    public:
        void clear();
        bool check(u32 flag) const;
        void set(u32 flag);
        void reset(u32 flag);

        u32 flags_;
    };

    class Extensions
    {
    public:
        void initialize();
        bool requiredOut() const
        {
            return false;
        }
    };

    class Extras
    {
    public:
        void initialize();
        bool requiredOut() const
        {
            return false;
        }
    };

    class Indices
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 bufferView_; //required
        s32 byteOffset_; //default:0
        s32 componentType_; //required
        Extensions extensions_;
        Extras extras_;
    };

    class Values
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 bufferView_; //required
        s32 byteOffset_; //default:0
        Extensions extensions_;
        Extras extras_;
    };

    class Sparse
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 count_; //required
        Indices indices_; //required
        Values values_; //required
        Extensions extensions_;
        Extras extras_;
    };

    class Accessor
    {
    public:
        static const u32 Flag_Min = 0x01U<<0;
        static const u32 Flag_Max = 0x01U<<1;

        void initialize();
        boolean checkRequirements() const;

        Flags flags_;
        s32 bufferView_;
        s32 byteOffset_; //default:0
        s32 componentType_; //required
        boolean normalized_; //default:false
        s32 count_; //required
        s32 type_; //required
        Number min_[16];
        Number max_[16];
        Sparse sparse_;
        String name_;
        Extensions extensions_;
        Extras extras_;
    };

    class Target
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 node_;
        String path_; //required
        Extensions extensions_;
        Extras extras_;
    };

    class Channel
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 sampler_; //required
        Target target_; //required
        Extensions extensions_;
        Extras extras_;
    };

    class AnimationSampler
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 input_; //required
        s32 interpolation_; //default:"LINEAR", "STEP", "CATMULLROMSPLINE", "CUBICSPLINE"
        s32 output_; //required
        Extensions extensions_;
        Extras extras_;
    };

    class Animation
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        Array<Channel> channels_;  //required
        Array<AnimationSampler> samplers_; //required
        String name_;
        Extensions extensions_;
        Extras extras_;
    };

    class Asset
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        String copyright_;
        String generator_;
        String version_; //required
        String minVersion_;
        Extensions extensions_;
        Extras extras_;
    };

    class Buffer
    {
    public:
        void initialize();

        boolean checkRequirements() const;

        String uri_;
        s32 byteLength_; //required
        String name_;
        Extensions extensions_;
        Extras extras_;
        u8* data_;
    };

    boolean load(Buffer& buffer, const String& directory);
    boolean save(String& uri, const Buffer& buffer, const String& directory);

    class BufferView
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 buffer_; //required
        s32 byteOffset_; //default:0;
        s32 byteLength_; //required
        s32 byteStride_;
        s32 target_;
        String name_;
        Extensions extensions_;
        Extras extras_;
    };

    class Orthographic
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        f32 xmag_; //required
        f32 ymag_; //required
        f32 zfar_; //required
        f32 znear_; //required
        Extensions extensions_;
        Extras extras_;
    };

    class Perspective
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        f32 aspectRatio_; //required
        f32 yfov_; //required
        f32 zfar_;
        f32 znear_; //required
        Extensions extensions_;
        Extras extras_;
    };

    class Camera
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        Orthographic orthographic_;
        Perspective perspective_;
        //String type_; //required
        s32 type_; //required
        String name_;
        Extensions extensions_;
        Extras extras_;
    };

    class Image
    {
    public:
        void initialize();

        String uri_;
        String mimeType_;
        s32 bufferView_;
        String name_;
        Extensions extensions_;
        Extras extras_;
    };

    class TextureInfo
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 index_; //required
        s32 texCoord_; //default:0
        Extensions extensions_;
        Extras extras_;
    };

    class NormalTextureInfo
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 index_; //required
        s32 texCoord_; //default:0
        f32 scale_; //default:1
        Extensions extensions_;
        Extras extras_;
    };

    class OcclusionTextureInfo
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 index_; //required
        s32 texCoord_; //default:0
        f32 strength_; //default:1
        Extensions extensions_;
        Extras extras_;
    };

    class PbrMetallicRoughness
    {
    public:
        void initialize();

        f32 baseColorFactor_[4]; //default:[1,1,1,1]
        TextureInfo baseColorTexture_;
        f32 metallicFactor_; //default:1
        f32 roughnessFactor_; //default:1
        TextureInfo metallicRoughnessTexture_;
        Extensions extensions_;
        Extras extras_;
    };

    class Material
    {
    public:
        void initialize();

        String name_;
        PbrMetallicRoughness pbrMetallicRoughness_;
        NormalTextureInfo normalTexture_;
        OcclusionTextureInfo occlusionTexture_;
        TextureInfo emissiveTexture_;
        f32 emissiveFactor_[3]; //default:[0,0,0]
        s32 alphaMode_; //defalt:OPAQUE, MASK, BLEND
        f32 alphaCutoff_; //default:0.5
        boolean doubleSided_; //default:false
        Extensions extensions_;
        Extras extras_;
    };

    class Attribute
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s8 semanticType_;
        s8 semanticIndex_;
        s16 accessor_;
    };

    class MorphTarget
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 indices_[3]; //POSITION, NORMAL, TANGENT
    };

    class Primitive
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        Array<Attribute> attributes_;
        s32 indices_;
        s32 material_;
        s32 mode_; //default:4 0 POINTS 1 LINES 2 LINE_LOOP 3 LINE_STRIP 4 TRIANGLES 5 TRIANGLE_STRIP 6 TRIANGLE_FAN
        Array<MorphTarget> targets_;
        Extensions extensions_;
        Extras extras_;
    };

    class Mesh
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        Array<Primitive> primitives_; //required
        Array<f32> weights_;
        String name_;
        Extensions extensions_;
        Extras extras_;
    };

    class Node
    {
    public:
        static const s32 Flag_Matrix = (0x01<<0);
        static const s32 Flag_Rotation = (0x01<<1);
        static const s32 Flag_Scale = (0x01<<2);
        static const s32 Flag_Translation = (0x01<<3);

        void initialize();

        Flags flags_;
        s32 camera_;
        Array<s32> children_;
        s32 skin_;
        f32 matrix_[16]; //default:identity
        s32 mesh_;
        f32 rotation_[4]; //default:[0,0,0,1]
        f32 scale_[3]; //default:[1,1,1]
        f32 translation_[3]; //default:[0,0,0]
        Array<f32> weights_;
        String name_;
        Extensions extensions_;
        Extras extras_;
    };

    class Sampler
    {
    public:
        void initialize();

        s32 magFilter_;
        s32 minFilter_;
        s32 wrapS_; //default:REPEAT
        s32 wrapT_; //default:REPEAT
        String name_;
        Extensions extensions_;
        Extras extras_;
    };

    class Scene
    {
    public:
        void initialize();

        Array<s32> nodes_;
        String name_;
        Extensions extensions_;
        Extras extras_;
    };

    class Skin
    {
    public:
        void initialize();
        boolean checkRequirements() const;

        s32 inverseBindMatrices_;
        s32 skeleton_;
        Array<s32> joints_; //required
        String name_;
        Extensions extensions_;
        Extras extras_;
    };

    class Texture
    {
    public:
        void initialize();

        s32 sampler_;
        s32 source_;
        String name_;
        Extensions extensions_;
        Extras extras_;
    };

    //---------------------------------------------------------------
    class glTF
    {
    public:
        struct SortNode
        {
            s32 parent_; ///< parent's index. If this is root, parent_ is -1
            s32 oldId_; ///< index in a Node array
            s32 numChildren_; ///< number of children
            s32 childrenStart_; ///< start index of children
        };

        glTF();
        ~glTF();

        void initialize();
        void setDirectory(const Char* directory);
        void sortNodes();

        boolean loadBuffers();
        boolean loadGLBBuffers();

        boolean checkRequirements() const;

        String directory_;

        Array<String> extensionsUsed_;
        Array<String> extensionsRequired_;
        Array<Accessor> accessors_;
        Array<Animation> animations_;
        Asset asset_;
        Array<Buffer> buffers_;
        Array<BufferView> bufferViews_;
        Array<Camera> cameras_;
        Array<Image> images_;
        Array<Material> materials_;
        Array<Mesh> meshes_;
        Array<Node> nodes_;
        Array<SortNode> sortedNodes_; 
        Array<Sampler> samplers_;
        s32 scene_;
        Array<Scene> scenes_;
        Array<Skin> skins_;
        Array<Texture> textures_;
        Extensions extensions_;
        Extras extras_;

        void allocate(u32 size);
        u32 size() const;
        const u8* getBin(u32 offset) const;
        u8* getBin(u32 offset);

        void allocateGLB(u32 size);
        u32 sizeGLB() const;
        const u8* getGLB(u32 offset) const;
        u8* getGLB(u32 offset);
    private:
        glTF(const glTF&) =delete;
        glTF& operator=(const glTF&) =delete;

        static boolean isRoot(s32 node, const Array<Node>& nodes);
        static void addChildren(s32 parent, Array<SortNode>& dst, const Array<Node>& nodes);

        u32 size_;
        u8* bin_;
        u32 glbSize_;
        u8* glbBin_;
    };

    //---------------------------------------------------------------
    //---
    //--- glTFHandler
    //---
    //---------------------------------------------------------------
    class glTFHandler : public glTFBase
    {
    public:
        glTFHandler();
        glTFHandler(const Char* directory);
        virtual ~glTFHandler();

        inline const glTF& get() const;
        inline glTF& get();
    protected:
        void parseGlTF(const JSAny& glTF) override;

        virtual void parseAccessor(Accessor& accessor, const JSAny& value);
        virtual void parseSparse(Sparse& sparse, const JSAny& value);
        virtual void parseIndices(Indices& indices, const JSAny& value);
        virtual void parseValues(Values& values, const JSAny& value);
        virtual void parseAnimation(Animation& animation, const JSAny& value);
        virtual void parseChannel(Channel& channel, const JSAny& value);
        virtual void parseTarget(Target& target, const JSAny& value);
        virtual void parseAnimationSampler(AnimationSampler& sampler, const JSAny& value);
        virtual void parseAsset(Asset& asset, const JSAny& value);
        virtual void parseBuffer(Buffer& buffer, const JSAny& value);
        virtual void parseBufferView(BufferView& bufferView, const JSAny& value);
        virtual void parseCamera(Camera& camera, const JSAny& value);
        virtual void parseOrthographic(Orthographic& orthographic, const JSAny& value);
        virtual void parsePerspective(Perspective& perspective, const JSAny& value);
        virtual void parseImage(Image& image, const JSAny& value);
        virtual void parseMaterial(Material& material, const JSAny& value);
        virtual void parsePbrMetallicRoughness(PbrMetallicRoughness& pbrMetallicRoughness, const JSAny& value);
        virtual void parseNormalTexture(NormalTextureInfo& normal, const JSAny& value);
        virtual void parseOcclusionTexture(OcclusionTextureInfo& occlusion, const JSAny& value);
        virtual void parseMesh(Mesh& mesh, const JSAny& value);
        virtual void parsePrimitive(Primitive& primitive, const JSAny& value);
        virtual void parseAttribute(Attribute& attribute, const JSKeyValue& keyValue);
        virtual void parseMorphTarget(MorphTarget& morphTarget, const JSAny& value);
        virtual void parseNode(Node& node, const JSAny& value);
        virtual void parseSampler(Sampler& sampler, const JSAny& value);
        virtual void parseScene(Scene& scene, const JSAny& value);
        virtual void parseSkin(Skin& skin, const JSAny& value);
        virtual void parseTexture(Texture& texture, const JSAny& value);
        virtual void parseTextureInfo(TextureInfo& texture, const JSAny& value);
        virtual void parseExtensions(Extensions& extensions, const JSAny& value);
        virtual void parseExtras(Extras& extras, const JSAny& value);

        glTF gltf_;
    };

    inline const glTF& glTFHandler::get() const
    {
        return gltf_;
    }

    inline glTF& glTFHandler::get()
    {
        return gltf_;
    }

    //---------------------------------------------------------------
    //---
    //--- GLBEventHandler
    //---
    //---------------------------------------------------------------
    class GLBEventHandler : public glTFHandler
    {
    public:
        GLBEventHandler();
        virtual ~GLBEventHandler();

        virtual bool parseHeader(u32 magic, u32 version, u32 length);

    private:
        GLBEventHandler(const GLBEventHandler&) = delete;
        GLBEventHandler& operator=(const GLBEventHandler&) = delete;
    };

    //---------------------------------------------------------------
    //---
    //--- GLBReader
    //---
    //---------------------------------------------------------------
    class GLBReader : public JSONReader
    {
    public:
        static const u32 Magic = 0x46546C67U;//'FTlg';
        static const u32 Version = 2;
        static const u32 ChunkType_JSON = 0x4E4F534AU;
        static const u32 ChunkType_BIN  = 0x004E4942U;
        struct Header
        {
            u32 magic_;
            u32 version_;
            u32 length_;
        };

        struct Chunk
        {
            u32 length_;
            u32 type_;
        };

        GLBReader(IStream& istream, GLBEventHandler& handler);
        ~GLBReader();

        bool read(s32 flags=0);
    private:
        bool readChunkJSON(u32& size);
        bool readChunkBIN(u32& size);

        GLBEventHandler& handler_;
    };


    //---------------------------------------------------------------
    //---
    //--- glTFWriter
    //---
    //---------------------------------------------------------------
    class glTFWriter
    {
    public:
        static const u32 Flag_Format = 0x01U<<0; 

        struct Indent
        {
            Indent(s32& indent)
                :indent_(indent)
            {
                ++indent_;
            }
            ~Indent()
            {
                --indent_;
            }
            s32& indent_;
        };

        glTFWriter(OStream& ostream);
        ~glTFWriter();

        /**
        @param gltf
        @param type ... file type
        */
        bool write(const glTF& gltf, GLTF_FILE type, u32 flags);
    private:
        void printIndent();
        void printLine();
        void printSeparator();
        void printSeparatorLine();
        void printKeyValueSeparator();
        void replaceLastLine();
        void format(const Char* format, ...);
        bool print(const Char* str);
        bool print(const String& str);
        bool print(s32 value);
        bool print(u32 value);
        bool print(f32 value);
        bool print(boolean value);
        void printNull();

        void beginObject();
        void endObject();
        void beginArray();
        void endArray();

        void printObjectProperty(const Char* key, const String& value);
        void printObjectProperty(const Char* key, const Char* value);
        void printObjectProperty(const Char* key, s32 value);
        void printObjectProperty(const Char* key, f32 value);
        void printObjectProperty(const Char* key, s32 num, const f32* value);
        void printObjectProperty(const Char* key, s32 num, const s32* value);
        void printObjectProperty(const Char* key, s32 num, const u32* value);

        void printObjectProperty1(const Char* key, const f32* value)
        {
            printObjectProperty(key, 1, value);
        }
        void printObjectProperty2(const Char* key, const f32* value)
        {
            printObjectProperty(key, 2, value);
        }
        void printObjectProperty3(const Char* key, const f32* value)
        {
            printObjectProperty(key, 3, value);
        }
        void printObjectProperty4(const Char* key, const f32* value)
        {
            printObjectProperty(key, 4, value);
        }
        void printObjectProperty9(const Char* key, const f32* value)
        {
            printObjectProperty(key, 9, value);
        }
        void printObjectProperty16(const Char* key, const f32* value)
        {
            printObjectProperty(key, 16, value);
        }

        void printObjectProperty(s32 componentType, s32 type, const Char* key, const Number* value);
        void printObjectProperty(const Char* key, boolean value);

        template<class T>
        bool printObjectProperty(const Char* key, const T& x);

        template<class T>
        bool printArray(const Char* key, const Array<T>& array);

        bool print(const Accessor& accessor);
        bool print(const Sparse& sparse);
        bool print(const Indices& indices);
        bool print(const Values& values);
        bool print(const Animation& animation);
        bool print(const Channel& channel);
        bool print(const AnimationSampler& animSampler);
        bool print(const Target& target);
        bool print(const Asset& asset);
        bool print(const Buffer& buffer);
        bool print(const BufferView& bufferView);
        bool print(const Camera& camera);
        bool print(const Orthographic& orthographic);
        bool print(const Perspective& perspective);
        bool print(const Extensions& extensions);
        bool print(const Extras& extras);
        bool print(const Image& image);
        bool print(const Material& material);
        bool print(const NormalTextureInfo& normalTextureInfo);
        bool print(const OcclusionTextureInfo& occlusionTextureInfo);
        bool print(const PbrMetallicRoughness& pbrMetallicRoughness);
        bool print(const Mesh& mesh);
        bool print(const Primitive& primitive);
        bool print(const Char* key, const Array<Attribute>& attributes);
        bool print(const Char* key, const Array<MorphTarget>& targets);
        bool print(const Node& node);
        bool print(const Sampler& sampler);
        bool print(const Scene& scene);
        bool print(const Skin& skin);
        bool print(const Texture& texture);
        bool print(const TextureInfo& textureInfo);

        Flags flags_;
        const glTF* gltf_;
        s32 indent_;
        OStream& ostream0_;
        OStream* ostream1_;
    };

    template<class T>
    bool glTFWriter::printObjectProperty(const Char* key, const T& x)
    {
        printIndent();
        print(key);
        printKeyValueSeparator();
        if(!print(x)){
            return false;
        }
        printSeparatorLine();
        return true;
    }

    template<class T>
    bool glTFWriter::printArray(const Char* key, const Array<T>& array)
    {
        if(array.size()<=0){
            return false;
        }
        printIndent();
        print(key);
        printKeyValueSeparator();
        beginArray();
        {
            Indent indent(indent_);
            printIndent();
            print(array[0]);
            for(s32 i=1; i<array.size(); ++i){
                printSeparator();
                printLine();
                printIndent();
                if(!print(array[i])){
                    return false;
                }
            }
        }
        printLine();
        endArray();
        printSeparatorLine();
        return true;
    }
}
#endif //INC_CPPGLTF_H_

#ifdef CPPGLTF_IMPLEMENTATION
namespace cppgltf
{
namespace
{
    static const s8 Base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const s8 Base64PadChar = '=';
    s32 getIndexBase64Chars(s8 c)
    {
        if('A'<=c && c<='Z'){
            return c-'A';

        }else if('a'<=c && c<='z'){
            return c-'a' + 26;

        }else if('0'<=c && c<='9'){
            return c-'0' + 52;
        }
        for(s32 i=62; i<64; ++i){
            if(Base64Chars[i]==c){
                return i;
            }
        }
        return 0;
    }
}

    boolean isBase64(s32 c)
    {
        return std::isalnum(c) || '+'==c || '/'==c;
    }

    s32 getLengthEncodedBase64(s32 l)
    {
        l = l<<3;
        s32 n0 = l/6;
        if(0<(l-n0*6)){
            ++n0;
        }
        s32 n1 = ((n0 + 3)>>2)<<2;
        return n1;
    }

    s32 getLengthDecodedBase64(s32 l)
    {
        return (l*6 + 7)>>3;
    }

    s32 decodeBase64(u8* dst, FILE* file)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != dst);
        CPPGLTF_ASSERT(CPPGLTF_NULL != file);

        s32 l=0;
        s32 d=0;
        u8 tmp[4];
        while(!feof(file)){
            s32 c = fgetc(file);
            if(c<0||!isBase64(c) || Base64PadChar==c){
                break;
            }
            tmp[l] =static_cast<u8>(getIndexBase64Chars(static_cast<s8>(c)));
            ++l;
            if(4<=l){
                dst[d+0] = (tmp[0]<<2) | ((tmp[1]&0x30U)>>4);
                dst[d+1] = ((tmp[1]&0x0FU)<<4) | ((tmp[2]&0x3CU)>>2);
                dst[d+2] = ((tmp[2]&0x03U)<<6) | tmp[3];
                d += 3;
                l=0;
            }
        }
        if(0<l){
            for(s32 i=l; i<4; ++i){
                tmp[i] = 0;
            }
            dst[d+0] = (tmp[0]<<2) | ((tmp[1]&0x30U)>>4);
            dst[d+1] = ((tmp[1]&0x0FU)<<4) | ((tmp[2]&0x3CU)>>2);
            dst[d+2] = ((tmp[2]&0x03U)<<6) | tmp[3];
            d += (l*6)>>3;
        }
        return d;
    }

    s32 decodeBase64(u8* dst, s32 length, const s8* src)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != dst);
        CPPGLTF_ASSERT(0<=length);
        CPPGLTF_ASSERT(CPPGLTF_NULL != src);

        s32 l=0;
        s32 d=0;
        u8 tmp[4];
        for(s32 i=0;
            i<length && isBase64(src[i]) && Base64PadChar != src[i];
            ++i)
        {
            tmp[l] = static_cast<u8>(getIndexBase64Chars(src[i]));
            ++l;
            if(4<=l){
                dst[d+0] = (tmp[0]<<2) | ((tmp[1]&0x30U)>>4);
                dst[d+1] = (tmp[1]<<4) | ((tmp[2]&0x3CU)>>2);
                dst[d+2] = (tmp[2]<<6) | tmp[3];
                d += 3;
                l=0;
            }
        }
        if(0<l){
            for(s32 i=l; i<4; ++i){
                tmp[i] = 0;
            }
            dst[d+0] = (tmp[0]<<2) | ((tmp[1]&0x30U)>>4);
            dst[d+1] = (tmp[1]<<4) | ((tmp[2]&0x3CU)>>2);
            dst[d+2] = (tmp[2]<<6) | tmp[3];
            d += (l*6)>>3;
        }
        return d;
    }

    s32 encodeBase64(FILE* file, s32 length, const u8* src)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != file);
        CPPGLTF_ASSERT(0<=length);
        CPPGLTF_ASSERT(CPPGLTF_NULL != src);

        u8 tmp[3];
        s8 dst[4];
        s32 d=0;
        s32 l=0;
        for(s32 i=0; i<length; ++i){
            tmp[l] = src[i];
            ++l;
            if(3<=l){
                dst[0] = Base64Chars[tmp[0]>>2];
                dst[1] = Base64Chars[((tmp[0]&0x03U)<<4) | (tmp[1]>>4)];
                dst[2] = Base64Chars[((tmp[1]&0x0FU)<<2) | (tmp[2]>>6)];
                dst[3] = Base64Chars[(tmp[2]&0x3FU)];
                if(fwrite(dst, 4, 1, file)<0){
                    return d;
                }
                d += 4;
                l = 0;
            }
        }
        if(0<l){
            for(s32 j=l; j<3; ++j){
                tmp[j] = 0;
            }
            dst[0] = Base64Chars[tmp[0]>>2];
            dst[1] = Base64Chars[((tmp[0]&0x03U)<<4) | (tmp[1]>>4)];
            dst[2] = Base64Chars[((tmp[1]&0x0FU)<<2) | (tmp[2]>>6)];
            dst[3] = Base64Chars[(tmp[2]&0x3FU)];
            s32 n = l<<3;
            s32 r = (l<<3)/6;
            if(0<(n-r*6)){
                ++l;
            }
            for(s32 j=l; j<4; ++j){
                dst[j] = Base64PadChar;
            }
            if(fwrite(dst, 4, 1, file)<0){
                return d;
            }
            d+=4;
        }
        return d;
    }

    s32 encodeBase64(s8* dst, s32 length, const u8* src)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != dst);
        CPPGLTF_ASSERT(0<=length);
        CPPGLTF_ASSERT(CPPGLTF_NULL != src);

        u8 tmp[3];
        s32 d=0;
        s32 l=0;
        for(s32 i=0; i<length; ++i){
            tmp[l] = src[i];
            ++l;
            if(3<=l){
                dst[d+0] = Base64Chars[tmp[0]>>2];
                dst[d+1] = Base64Chars[((tmp[0]&0x03U)<<4) | (tmp[1]>>4)];
                dst[d+2] = Base64Chars[((tmp[1]&0x0FU)<<2) | (tmp[2]>>6)];
                dst[d+3] = Base64Chars[(tmp[2]&0x3FU)];
                d += 4;
                l = 0;
            }
        }
        if(0<l){
            for(s32 j=l; j<3; ++j){
                tmp[j] = 0;
            }
            dst[d+0] = Base64Chars[tmp[0]>>2];
            dst[d+1] = Base64Chars[((tmp[0]&0x03U)<<4) | (tmp[1]>>4)];
            dst[d+2] = Base64Chars[((tmp[1]&0x0FU)<<2) | (tmp[2]>>6)];
            dst[d+3] = Base64Chars[(tmp[2]&0x3FU)];
            s32 n = l<<3;
            s32 r = (l<<3)/6;
            if(0<(n-r*6)){
                ++l;
            }
            for(s32 j=l; j<4; ++j){
                dst[d+j] = Base64PadChar;
            }
            d+=4;
        }
        return d;
    }

    f32 toFloat(s8 x)
    {
        return maximum(x/127.0f, -1.0f);
    }

    f32 toFloat(u8 x)
    {
        return x/255.0f;
    }

    f32 toFloat(s16 x)
    {
        return maximum(x/32767.0f, -1.0f);
    }

    f32 toFloat(u16 x)
    {
        return x/65535.0f;
    }

    s8 toS8(f32 x)
    {
        return static_cast<s8>(round(x*127.0f));
    }

    u8 toU8(f32 x)
    {
        return static_cast<u8>(round(x*255.0f));
    }

    s16 toS16(f32 x)
    {
        return static_cast<s16>(round(x*32767.0f));
    }

    u16 toU16(f32 x)
    {
        return static_cast<u16>(round(x*65535.0f));
    }

    //--------------------------------------------
    //---
    //--- String
    //---
    //--------------------------------------------
    String::String()
        :capacity_(ExpandSize)
        ,length_(0)
    {
        buffer_.small_[0] = '\0';
    }

    String::String(String&& rhs)
        :capacity_(rhs.capacity_)
        ,length_(rhs.length_)
        ,buffer_(rhs.buffer_)
    {
        rhs.capacity_ = 0;
        rhs.length_ = 0;
        rhs.buffer_.elements_ = CPPGLTF_NULL;
    }

    String::~String()
    {
        if(ExpandSize<capacity_){
            CPPGLTF_FREE(buffer_.elements_);
            buffer_.elements_ = CPPGLTF_NULL;
        }
        capacity_ = 0;
        length_ = 0;
    }

    void String::clear()
    {
        length_ = 0;
        Char* buffer = getBuffer();
        buffer[0] = '\0';
    }

    void String::resize(s32 length)
    {
        CPPGLTF_ASSERT(0<=length);
        reserve(length+1);
        length_ = length;
    }

    void String::reserve(s32 capacity)
    {
        CPPGLTF_ASSERT(0<=capacity);
        length_ = 0;
        if(capacity<=capacity_) {
            Char* buffer = getBuffer();
            buffer[0] = '\0';
            return;
        }
        if(ExpandSize<capacity_) {
            CPPGLTF_FREE(buffer_.elements_);
        }
        if(capacity<=ExpandSize){
            capacity_ = ExpandSize;
            buffer_.small_[0] = '\0';
            return;
        }

        capacity_ = getCapacity(capacity);
        buffer_.elements_ = (Char*)CPPGLTF_MALLOC(capacity_);
        buffer_.elements_[0] = '\0';
    }

    String& String::operator=(String&& rhs)
    {
        if(this == &rhs){
            return *this;
        }
        capacity_ = rhs.capacity_;
        length_ = rhs.length_;
        buffer_ = rhs.buffer_;

        rhs.capacity_ = 0;
        rhs.length_ = 0;
        rhs.buffer_.elements_ = CPPGLTF_NULL;
        return *this;
    }

    String& String::operator=(const Char* str)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != str);
        s32 length = static_cast<s32>(::strlen(str));
        assign(length, str);
        return *this;
    }

    void String::assign(s32 length, const Char* str)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != str);
        CPPGLTF_ASSERT(static_cast<size_t>(length)<=::strlen(str));
        if(length<=0){
            clear();
            return;
        }
        createBuffer(length);
        length_ = length;
        Char* buffer = getBuffer();
        ::memcpy(buffer, str, sizeof(Char)*length_);
        buffer[length_] = '\0';
    }

    void String::push_back(Char c)
    {
        expandBuffer(length_+1);
        Char* buffer = getBuffer();
        buffer[length_] = c;
        ++length_;
        buffer[length_] = '\0';
    }

    void String::initBuffer(s32 length)
    {
        if(length<ExpandSize){
            capacity_ = ExpandSize;
            length_ = length;
            buffer_.small_[length_] = '\0';
            return;
        }
        s32 capacity = length+1;
        capacity_ = getCapacity(capacity);
        length_ = length;
        buffer_.elements_ = (Char*)CPPGLTF_MALLOC(capacity_);
        buffer_.elements_[length_] = '\0';
    }

    void String::createBuffer(s32 length)
    {
        s32 capacity = length+1;
        if(capacity<=capacity_){
            return;
        }
        if(capacity<=ExpandSize){
            capacity_ = ExpandSize;
            return;
        }
        if(ExpandSize<capacity_){
            CPPGLTF_FREE(buffer_.elements_);
        }

        capacity_ = getCapacity(capacity);
        buffer_.elements_ = (Char*)CPPGLTF_MALLOC(capacity_);
    }

    void String::expandBuffer(s32 length)
    {
        s32 capacity = length+1;
        if(capacity<=capacity_) {
            return;
        }
        if(capacity<=ExpandSize){
            capacity_ = ExpandSize;
            return;
        }
        capacity = getCapacity(capacity);
        Char* elements = (Char*)CPPGLTF_MALLOC(capacity);
        Char* buffer = getBuffer();
        ::memcpy(elements, buffer, sizeof(Char)*capacity_);
        if(ExpandSize<capacity_){
            CPPGLTF_FREE(buffer_.elements_);
        }

        capacity_ = capacity;
        buffer_.elements_ = elements;
    }

    boolean String::operator==(const String& rhs) const
    {
        return (length() == rhs.length())? (0 == ::strncmp(c_str(), rhs.c_str(), length())) : false;
    }

    boolean String::startWith(const Char* str) const
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != str);
        const Char* buffer = getBuffer();
        for(s32 i=0; i<length(); ++i){
            if(str[i] == '\0'){
                return true;
            }
            if(buffer[i] != str[i]){
                return false;
            }
        }
        return false;
    }

    boolean operator==(const String& lhs, const Char* rhs)
    {
        s32 len = static_cast<s32>(::strlen(rhs));
        return (lhs.length() == len)? (0 == ::strncmp(lhs.c_str(), rhs, len)) : false;
    }

    boolean operator==(const Char* lhs, const String& rhs)
    {
        s32 len = static_cast<s32>(::strlen(lhs));
        return (len == rhs.length())? (0 == ::strncmp(lhs, rhs.c_str(), len)) : false;
    }

    //---------------------------------------------------------------
    //---
    //--- IStream
    //---
    //---------------------------------------------------------------
    ISStream::ISStream()
        :pos_(0)
        ,length_(0)
        ,str_(CPPGLTF_NULL)
    {
    }

    ISStream::ISStream(ISStream&& rhs)
        :pos_(rhs.pos_)
        ,length_(rhs.length_)
        ,str_(rhs.str_)
    {
        rhs.pos_ = 0;
        rhs.length_ = 0;
        rhs.str_ = CPPGLTF_NULL;
    }

    ISStream::ISStream(off_t length, const u8* str)
        :pos_(0)
        ,length_(length)
        ,str_(str)
    {
        CPPGLTF_ASSERT(0<=length_);
        CPPGLTF_ASSERT(CPPGLTF_NULL != str_);
    }

    ISStream::~ISStream()
    {
        pos_ = 0;
        length_ = 0;
        str_ = CPPGLTF_NULL;
    }

    off_t ISStream::tell() const
    {
        return pos_;
    }

    boolean ISStream::seek(off_t pos)
    {
        CPPGLTF_ASSERT(0<=pos && pos<length_);
        pos_ = pos;
        return true;
    }

    s32 ISStream::get()
    {
        s32 ret=-1;
        if(pos_<length_){
            ret = str_[pos_];
            ++pos_;
        }
        return ret;
    }

    s32 ISStream::unget(s32 c)
    {
        --pos_;
        CPPGLTF_ASSERT(0<=pos_ && pos_<length_);
        CPPGLTF_ASSERT(str_[pos_] == c);
        return c;
    }

    s32 ISStream::read(u8* dst, s32 size)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != dst);
        off_t end = pos_+size;
        if(length_<end){
            return 0;
        }
        for(s32 i=0; i<size; ++i){
            dst[i] = str_[pos_];
            ++pos_;
        }
        return 1;
    }

    boolean ISStream::isEOF() const
    {
        return length_<=pos_;
    }

    ISStream& ISStream::operator=(ISStream&& rhs)
    {
        if(this == &rhs){
            return *this;
        }
        pos_ = rhs.pos_;
        length_ = rhs.length_;
        str_ = rhs.str_;

        rhs.pos_ = 0;
        rhs.length_ = 0;
        rhs.str_ = CPPGLTF_NULL;
        return *this;
    }

    //---------------------------------------------------------------
    //---
    //--- IFStream
    //---
    //---------------------------------------------------------------
    IFStream::IFStream()
        :pos_(0)
    {
    }

    IFStream::IFStream(IFStream&& rhs)
        :parent_type(std::move(rhs))
        ,pos_(rhs.pos_)
    {
        rhs.pos_ = 0;
    }

    IFStream::~IFStream()
    {
        pos_ = 0;
    }

    bool IFStream::open(const Char* filepath)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != filepath);
        return parent_type::open(filepath, "rb");
    }

    off_t IFStream::tell() const
    {
#if _MSC_VER
        return CPPGLTF_FTELL(file_)-pos_;
#else
        return CPPGLTF_FTELL(file_)-pos_;
#endif
    }

    boolean IFStream::seek(off_t pos)
    {
#if _MSC_VER
        return 0<=CPPGLTF_FSEEK(file_, pos_+pos, SEEK_SET);
#else 
        return 0<=CPPGLTF_FSEEK(file_, pos_+pos, SEEK_SET);
#endif
    }

    s32 IFStream::get()
    {
        return fgetc(file_);
    }

    s32 IFStream::unget(s32 c)
    {
        return ungetc(c, file_);
    }

    s32 IFStream::read(u8* dst, s32 size)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != dst);
        return static_cast<s32>(fread(dst, size, 1, file_));
    }

    boolean IFStream::isEOF() const
    {
        return 0!=feof(file_);
    }

    IFStream& IFStream::operator=(IFStream&& rhs)
    {
        if(this == &rhs){
            return *this;
        }
        pos_ = rhs.pos_;
        file_ = rhs.file_;

        rhs.pos_ = 0;
        rhs.file_ = CPPGLTF_NULL;
        return *this;
    }

    //---------------------------------------------------------------
    //---
    //--- OSStream
    //---
    //---------------------------------------------------------------
    OSStream::OSStream()
        :capacity_(0)
        ,pos_(0)
        ,buffer_(CPPGLTF_NULL)
    {
    }

    OSStream::OSStream(s32 capacity)
        :capacity_(capacity)
        ,pos_(0)
        ,buffer_(CPPGLTF_NULL)
    {
        CPPGLTF_ASSERT(0<=capacity_);
        buffer_ = (u8*)CPPGLTF_MALLOC(capacity_);
    }

    OSStream::OSStream(OSStream&& rhs)
        :capacity_(rhs.capacity_)
        ,pos_(rhs.pos_)
        ,buffer_(rhs.buffer_)
    {
        rhs.capacity_ = 0;
        rhs.pos_ = 0;
        rhs.buffer_ = CPPGLTF_NULL;
    }

    OSStream::~OSStream()
    {
        capacity_ = 0;
        pos_ = 0;
        CPPGLTF_FREE(buffer_);
        buffer_ = CPPGLTF_NULL;
    }

    s32 OSStream::size() const
    {
        return pos_;
    }

    const u8* OSStream::buff() const
    {
        return buffer_;
    }

    s32 OSStream::write(s32 size, const u8* src)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != buffer_);
        CPPGLTF_ASSERT(0<=size);
        CPPGLTF_ASSERT(CPPGLTF_NULL != src);

        s32 p = pos_ + size;
        if(capacity_<p){
            s32 newCapacity = calcNext(p, capacity_);
            u8* buffer = (u8*)CPPGLTF_MALLOC(newCapacity);
            ::memcpy(buffer, buffer_, capacity_);
            CPPGLTF_FREE(buffer_);
            buffer_ = buffer;
            capacity_ = newCapacity;
        }
        ::memcpy(buffer_+pos_, src, size);
        pos_ = p;
        return 1;
    }

    boolean OSStream::replaceLast(s32 backLen, const Char* str)
    {
        CPPGLTF_ASSERT(0<backLen);
        CPPGLTF_ASSERT(CPPGLTF_NULL != str);
        s32 pos = pos_-backLen;
        if(pos<0){
            return false;
        }
        return 0<write(static_cast<s32>(strlen(str)), reinterpret_cast<const u8*>(str));
    }

    OSStream& OSStream::operator=(OSStream&& rhs)
    {
        if(this == &rhs){
            return *this;
        }
        capacity_ = rhs.capacity_;
        pos_ = rhs.pos_;
        buffer_ = rhs.buffer_;

        rhs.capacity_ = 0;
        rhs.pos_ = 0;
        rhs.buffer_ = CPPGLTF_NULL;
        return *this;
    }

    s32 OSStream::calcNext(s32 request, s32 capacity)
    {
        s32 nextCapacity = capacity;
        do{
            nextCapacity += 4*1024;
        }while(nextCapacity<request);
        return nextCapacity;
    }

    //---------------------------------------------------------------
    //---
    //--- OFStream
    //---
    //---------------------------------------------------------------
    OFStream::OFStream()
    {
    }

    OFStream::OFStream(OFStream&& rhs)
        :parent_type(std::move(rhs))
    {
    }

    OFStream::~OFStream()
    {
    }

    bool OFStream::open(const Char* filepath)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != filepath);
        return parent_type::open(filepath, "wb");
    }

    s32 OFStream::write(s32 size, const u8* src)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != file_);
        CPPGLTF_ASSERT(0<=size);
        CPPGLTF_ASSERT(CPPGLTF_NULL != src);
        return static_cast<s32>(fwrite(src, size, 1, file_));
    }

    boolean OFStream::replaceLast(s32 length, const Char* str)
    {
        CPPGLTF_ASSERT(0<length);
        CPPGLTF_ASSERT(CPPGLTF_NULL != str);
        if(CPPGLTF_FSEEK(file_, -length, SEEK_CUR)<0){
            return false;
        }
        return 0<fwrite(str, strlen(str), 1, file_);
    }

    OFStream& OFStream::operator=(OFStream&& rhs)
    {
        if(this == &rhs){
            return *this;
        }
        file_ = rhs.file_;

        rhs.file_ = CPPGLTF_NULL;
        return *this;
    }

    //---------------------------------------------------------------
    //---
    //--- RangeStream
    //---
    //---------------------------------------------------------------
    RangeStream::RangeStream(IStream* stream, const Range& range)
        :stream_(stream)
        ,start_(range.start_)
        ,length_(range.length_)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != stream_);
        CPPGLTF_ASSERT(0<=start_);
        CPPGLTF_ASSERT(0<=length_);
    }

    off_t RangeStream::length() const
    {
        return length_;
    }

    s32 RangeStream::read(u8* dst)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != dst);
        off_t pos = stream_->tell();
        if(!stream_->seek(start_)){
            return 0;
        }
        s32 ret = stream_->read(dst, length_);
        if(!stream_->seek(pos)){
            return 0;
        }
        return ret;
    }

    s32 RangeStream::readAsString(Char* str)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != str);
        off_t pos = stream_->tell();
        if(!stream_->seek(start_)){
            return 0;
        }
        s32 ret = stream_->read(reinterpret_cast<u8*>(str), length_);
        if(!stream_->seek(pos)){
            return 0;
        }
        str[length_] = '\0';
        return ret;
    }

    s32 RangeStream::readAsString(String& str)
    {
        str.resize(length_);
        off_t pos = stream_->tell();
        if(!stream_->seek(start_)){
            return 0;
        }
        Char* c = str.c_str();
        s32 ret = stream_->read(reinterpret_cast<u8*>(c), length_);
        if(!stream_->seek(pos)){
            return 0;
        }
        c[length_] = '\0';
        return ret;
    }

    s32 RangeStream::readAsInt(s32 defaultValue)
    {
        static const s32 MaxSize = 32;
        CPPGLTF_ASSERT(length_<MaxSize);
        off_t pos = stream_->tell();
        if(!stream_->seek(start_)){
            return defaultValue;
        }
        Char buffer[MaxSize];
        s32 ret = stream_->read(reinterpret_cast<u8*>(buffer), length_);
        if(!stream_->seek(pos)){
            return defaultValue;
        }
        if(ret<=0){
            return defaultValue;
        }
        buffer[length_] = '\0';
        return atol(buffer);
    }

    f32 RangeStream::readAsFloat(f32 defaultValue)
    {
        static const s32 MaxSize = 32;
        CPPGLTF_ASSERT(length_<MaxSize);
        off_t pos = stream_->tell();
        if(!stream_->seek(start_)){
            return defaultValue;
        }
        Char buffer[MaxSize];
        s32 ret = stream_->read(reinterpret_cast<u8*>(buffer), length_);
        if(!stream_->seek(pos)){
            return defaultValue;
        }
        if(ret<=0){
            return defaultValue;
        }
        buffer[length_] = '\0';
        return static_cast<f32>(atof(buffer));
    }

    //---------------------------------------------------------------
    //---
    //--- JSONReader
    //---
    //---------------------------------------------------------------
    JSONReader::JSONReader(IStream& istream, JSONEventHandler& handler)
        :flags_(0)
        ,istream_(istream)
        ,handler_(handler)
    {}

    JSONReader::~JSONReader()
    {
    }

    boolean JSONReader::read(s32 flags)
    {
        handler_.begin();
        flags_ = flags;
        Range value;
        if(!skipSpace()){
            if(istream_.isEOF()){
                return true;
            } else{
                onError();
                return false;
            }
        }
        s32 type = getValue(value);
        if(JSON_Error == type || (JSON_Array != type && JSON_Object != type)){
            return false;
        }

        //while(!istream_.isEOF()){
        //    s32 c = istream_.get();
        //    if(c==EOF){
        //        break;
        //    }
        //    if(!isspace(c)){
        //        return false;
        //    }
        //}

        handler_.root(type, RangeStream(&istream_, value));
        handler_.end();
        return true;
    }

    boolean JSONReader::skipSpace()
    {
        for(;;){
            s32 c = istream_.get();
            if(c<0){
                return false;
            }
            if(!isspace(c)){
                return EOF != istream_.unget(c);
            }
        }
    }

    boolean JSONReader::check(const s8* str)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != str);
        while('\0' != *str){
            s32 c = istream_.get();
            if(c<0){
                return false;
            }
            if(c != *str){
                return false;
            }
            ++str;
        }
        return true;
    }

    void JSONReader::setStart(Range& range)
    {
        range.start_ = istream_.tell();
        range.length_ = 0;
    }

    void JSONReader::setLength(Range& range)
    {
        range.length_ = istream_.tell()-range.start_-1;
    }

    s32 JSONReader::onError()
    {
        if(Flag_ErrorReported == (flags_&Flag_ErrorReported)){
            return JSON_Error;
        }
        flags_ |= Flag_ErrorReported;
        off_t pos = istream_.tell();
        if(!istream_.seek(0)){
            handler_.onError(-1, -1);
            return JSON_Error;
        }

        s32 line = 0;
        s32 count = 0;
        for(;;){
            s32 c = istream_.get();
            if(c == EOF){
                break;
            }
            if(c == '\r'){
                s32 c1 = istream_.get();
                if(c1 != '\n'){
                    if(EOF != c1){
                        istream_.unget(c1);
                    }
                }
                ++line;
                count = 0;
            }else if(c == '\n'){
                ++line;
                count = 0;
            }else{
                ++count;
            }
            if(istream_.tell() == pos){
                break;
            }
        }
        handler_.onError(line+1, count);
        return JSON_Error;
    }

    s32 JSONReader::getObject(Range& object)
    {
        setStart(object);
        handler_.beginObject();
        Range key;
        key.reset();
        boolean bKeyValue = false;
        boolean bSeparator = false;
        for(;;){
            if(!skipSpace()){
                return onError();
            }
            s32 c = istream_.get();
            switch(c){
            case JSONToken_OBJEnd:
                if(bSeparator && !bKeyValue){
                    return onError();
                }
                setLength(object);
                handler_.endObject(RangeStream(&istream_, object));
                return JSON_OK;

            case JSONToken_DQuote:
            {
                if(bKeyValue && !bSeparator){
                    return onError();
                }
                bKeyValue = true;
                bSeparator = false;
                if(JSON_Error == getString(key)){
                    return onError();
                }

                if(!skipSpace()){
                    return onError();
                }
                if(JSONToken_OBJColon != istream_.get()){
                    return onError();
                }
                if(!skipSpace()){
                    return onError();
                }
                Range value;
                s32 type = getValue(value);
                if(JSON_Error == type){
                    return JSON_Error;
                }
                handler_.keyValue(RangeStream(&istream_, key), type, RangeStream(&istream_, value));
            }
            break;
            case JSONToken_Separator:
                if(!bKeyValue){
                    return onError();
                }
                bKeyValue = false;
                bSeparator = true;
                break;
            default:
                return onError();
            }
        }
    }

    s32 JSONReader::getArray(Range& array)
    {
        setStart(array);
        handler_.beginArray();
        if(!skipSpace()){
            return onError();
        }
        s32 c = istream_.get();
        if(c<0){
            return onError();
        }
        switch(c){
        case JSONToken_ArrayEnd:
            setLength(array);
            handler_.endArray(RangeStream(&istream_, array));
            return JSON_OK;
        case JSONToken_Separator:
            return onError();
        default:
            break;
        }

        s32 prev = JSONToken_ArrayStart;
        for(;;){
            switch(c){
            case JSONToken_ArrayEnd:
                if(JSONToken_Separator == prev){
                    return onError();
                }
                setLength(array);
                handler_.endArray(RangeStream(&istream_, array));
                return JSON_OK;
            case JSONToken_Separator:
                if(JSONToken_Separator == prev){
                    return onError();
                }
                prev = JSONToken_Separator;
                break;
            default:
                if(JSONToken_Value == prev){
                    return onError();
                }
                prev = JSONToken_Value;
                if(EOF == istream_.unget(c)){
                    return onError();
                }
                Range value;
                s32 type = getValue(value);
                if(JSON_Error == type){
                    return JSON_Error;
                }
                handler_.value(type, RangeStream(&istream_, value));
                break;
            }
            if(!skipSpace()){
                return onError();
            }
            c = istream_.get();
            if(c==EOF){
                return onError();
            }
        }
    }

    s32 JSONReader::getValue(Range& value)
    {
        s32 c = istream_.get();
        if(c<0){
            return onError();
        }
        setStart(value);
        s32 type;
        switch(c)
        {
        case JSONToken_OBJStart:
            if(JSON_Error == getObject(value)){
                return onError();
            }
            type = JSON_Object;
            break;
        case JSONToken_ArrayStart:
            if(JSON_Error == getArray(value)){
                return onError();
            }
            type = JSON_Array;
            break;
        case JSONToken_DQuote:
            type = getString(value);
            break;
        case 't':
            if(!check(reinterpret_cast<const s8*>("rue"))){
                return onError();
            }
            type = JSON_True;
            setLength(value);
            break;
        case 'f':
            if(!check(reinterpret_cast<const s8*>("alse"))){
                return onError();
            }
            type = JSON_False;
            setLength(value);
            break;
        case 'n':
            if(!check(reinterpret_cast<const s8*>("ull"))){
                return onError();
            }
            type = JSON_Null;
            setLength(value);
            break;
        default:
            if(EOF == istream_.unget(c)){
                return onError();
            }
            type = getNumber(value);
            break;
        }
        return type;
    }

    s32 JSONReader::getString(Range& str)
    {
        setStart(str);
        for(;;){
            s32 c = istream_.get();
            if(c<0){
                return onError();
            }

            s32 n;
            if(0 == (c&0x80U)){//1byte
                switch(c){
                case JSONToken_DQuote:
                    setLength(str);
                    return JSON_String;
                case JSONToken_Escape:
                    c = istream_.get();
                    if(c<0){
                        return onError();
                    }
                    switch(c){
                    case JSONToken_DQuote:
                    case JSONToken_Escape:
                    case '/':
                    case 'b':
                    case 'f':
                    case 'n':
                    case 'r':
                    case 't':
                    case 'u':
                        break;
                    default:
                        return onError();
                    }
                    break;
                case '\t':
                case '\n':
                    return onError();
                }
                continue;

            }else if(6 == (c>>5)){//2byte
                n=1;
            }else if(14 == (c>>4)){//3byte
                n=2;
            }else if(30 == (c>>3)){//4byte
                n=3;
            }else if(62 == (c>>2)){//5byte
                n=4;
            }else if(126 == (c>>1)){//6byte
                n=5;
            }else{
                return onError();
            }
            for(s32 i=0; i<n; ++i){
                c = istream_.get();
                if(c<0 || 2 != (c>>6)){
                    return onError();
                }
            }
        }
    }

    s32 JSONReader::getNumber(Range& number)
    {
        setStart(number);
        s32 type = JSON_Integer;
        s32 c = istream_.get();
        if(c<0){
            return onError();
        }
        if(c=='-'){
            c = istream_.get();
        }
        if(!isNumber(c)){
            return onError();
        }
        if(c != '0'){
            if(JSON_Error == getDigit()){
                return JSON_Error;
            }
        }
        c = istream_.get();
        if(c == '.'){
            c = istream_.get();
            if(!isNumber(c)){
                return onError();
            }
            if(JSON_Error == getDigit()){
                return JSON_Error;
            }
            type = JSON_Float;
            c = istream_.get();
        }

        if(c=='e' || c=='E'){
            c = istream_.get();
            if(c=='+'||c=='-'){
                c = istream_.get();
            }
            if(!isNumber(c)){
                return onError();
            }
            istream_.unget(c);
            c = getDigit(false);
            if(JSON_Error == c){
                return JSON_Error;
            }
            type = JSON_Float;
        }else if(EOF==c){
            return onError();
        }
        setLength(number);
        return (EOF != istream_.unget(c))? type : onError();
    }

    s32 JSONReader::getDigit(boolean unget)
    {
        s32 c;
        for(;;){
            c = istream_.get();
            if(EOF==c){
                return onError();
            }
            if(!isNumber(c)){
                if(unget){
                    return (EOF != istream_.unget(c))? JSON_OK : onError();
                }
                break;
            }
        }
        return c;
    }

    boolean JSONReader::isNumber(s32 c) const
    {
        return ('0'<=c && c<='9');
    }
    boolean JSONReader::isOneToNine(s32 c) const
    {
        return ('1'<=c && c<='9');
    }

    //---------------------------------------------------------------
    //---
    //--- glTF JSON
    //---
    //---------------------------------------------------------------
    JSAny JSAny::createVector(Allocator& allocator, s32 num, s32 type)
    {
        JSAny x;
        x.type_ = type;
        x.num_ = num;
        switch(type){
        case JSON_Object:
            x.object_ = (JSKeyValue*)allocator.alloc(num);
            break;
        case JSON_Array:
            x.array_ = (JSAny*)allocator.alloc(num);
            break;
        default:
            CPPGLTF_ASSERT(false);
            break;
        }
        return x;
    }

    void JSAny::destroy(Allocator& allocator, JSAny& any)
    {
        switch(any.type_){
        case JSON_Object:
            for(s32 i=0; i<any.num_; ++i){
                JSKeyValue::destroy(allocator, any.object_[i]);
            }
            allocator.dealloc(any.num_, any.object_);
            break;
        case JSON_Array:
            for(s32 i=0; i<any.num_; ++i){
                JSAny::destroy(allocator, any.array_[i]);
            }
            allocator.dealloc(any.num_, any.array_);
            break;
        default:
            break;
        }
        any.num_ = 0;
        any.array_ = CPPGLTF_NULL;
    }


    void JSElement::destroy(Allocator& allocator, JSElement& element)
    {
        switch(element.value_.type_){
        case JSON_Object:
            for(s32 i=0; i<element.value_.num_; ++i){
                JSKeyValue::destroy(allocator, element.value_.object_[i]);
            }
            allocator.dealloc(element.capacity_, element.value_.object_);
            break;
        case JSON_Array:
            for(s32 i=0; i<element.value_.num_; ++i){
                JSAny::destroy(allocator, element.value_.array_[i]);
            }
            allocator.dealloc(element.capacity_, element.value_.array_);
            break;
        default:
            break;
        }
        element.capacity_ = 0;
        element.value_.num_ = 0;
        element.value_.array_ = CPPGLTF_NULL;
    }

    const s32 Allocator::SmaleSizes[] = {1,2,4,8,16,32,64,96,128,160};

    Allocator::Allocator()
        :large_(CPPGLTF_NULL)
        ,top_(CPPGLTF_NULL)
        ,largeTop_(CPPGLTF_NULL)
    {
        for(s32 i=0; i<TableSize; ++i){
            table_[i] = CPPGLTF_NULL;
        }
    }

    Allocator::~Allocator()
    {
        destroy();
    }

    void Allocator::destroy()
    {
        for(s32 i=0; i<TableSize; ++i){
            table_[i] = CPPGLTF_NULL;
        }
        large_ = CPPGLTF_NULL;
        while(CPPGLTF_NULL != top_){
            Page* next = top_->next_;
            CPPGLTF_FREE(top_);
            top_ = next;
        }
        while(CPPGLTF_NULL != largeTop_){
            Page* next = largeTop_->next_;
            CPPGLTF_FREE(largeTop_);
            largeTop_ = next;
        }
    }

    void* Allocator::alloc(s32 num)
    {
        CPPGLTF_ASSERT(0<num);
        if(num<=MaxNum){
            s32 index = 0;
            for(;SmaleSizes[index]<num;++index){
            }
            if(CPPGLTF_NULL != table_[index]){
                Page* page = table_[index];
                table_[index] = page->next_;
                return page;
            }
            return allocate(SmaleSizes[index]);

        }else{
            Page* prev = CPPGLTF_NULL;
            Page* page = large_;
            while(CPPGLTF_NULL != page){
                if(num<=large_->capacity_){
                    if(page == large_){
                        large_ = large_->next_;
                    }else{
                        prev->next_ = page->next_;
                    }
                    return page;
                }
            }
            PageMem* top = (PageMem*)CPPGLTF_MALLOC(UnitSize*(num+1));
            top->page_.capacity_ = 0;
            page = &(top+1)->page_;
            top->page_.next_ = largeTop_;
            largeTop_ = &(top->page_);
            return page;
        }
    }

    void Allocator::dealloc(s32 num, void* ptr)
    {
        if(num<=0 || CPPGLTF_NULL==ptr){
            return;
        }
        Page* page = (Page*)ptr;
        page->capacity_ = num;
        if(num<=MaxNum){
            s32 index = 0;
            for(;SmaleSizes[index]<num;++index){
            }
            page->next_ = table_[index];
            table_[index] = page;
        }else{
            page->next_ = large_;
            large_ = page;
        }
    }

    void* Allocator::allocate(s32 num)
    {
        Page* prev = CPPGLTF_NULL;
        Page* page = top_;
        while(CPPGLTF_NULL != page){
            if(num<=page->capacity_){
                s32 capacity = page->capacity_-num;
                s32 d = UnitNum-page->capacity_;
                if(0==capacity){
                    if(page == top_){
                        top_ = top_->next_;
                    } else{
                        prev->next_ = page->next_;
                    }
                }
                page->capacity_ = capacity;
                return &((PageMem*)page+d)->page_;
            }
            prev = page;
            page = page->next_;
        }

        PageMem* top = (PageMem*)CPPGLTF_MALLOC(UnitSize*UnitNum);
        top->page_.capacity_ = UnitNum-num-1;
        page = &(top+1)->page_;
        top->page_.next_ = top_;
        top_ = &(top->page_);
        return page;
    }

    namespace
    {
        struct Word{ u32 hash_; const char* str_;};
        static const s32 NumWords = 113;
        const Word words[] =
        {
            {3733770766U,"BUFFERVIEW"},
            {1736982884U,"BYTEOFFSET"},
            {535965544U,"COMPONENTTYPE"},
            {759490392U,"NORMALIZED"},
            {2750919380U,"COUNT"},
            {338683789U,"TYPE"},
            {475632249U,"MAX"},
            {239465655U,"MIN"},
            {413982885U,"SPARSE"},
            {1387956774U,"NAME"},
            {1482583691U,"EXTENSIONS"},
            {1617560430U,"EXTRAS"},
            {2180989045U,"CHANNELS"},
            {864986484U,"SAMPLERS"},
            {1638025307U,"INPUT"},
            {3457493885U,"INTERPOLATION"},
            {1469629700U,"OUTPUT"},
            {2970901150U,"COPYRIGHT"},
            {622175778U,"GENERATOR"},
            {3552295351U,"VERSION"},
            {3706427545U,"MINVERSION"},
            {2053729053U,"URI"},
            {1037972015U,"BYTELENGTH"},
            {4289874747U,"BUFFER"},
            {417073434U,"BYTESTRIDE"},
            {4080126536U,"TARGET"},
            {4050703019U,"ORTHOGRAPHIC"},
            {2423545653U,"PERSPECTIVE"},
            {4060151151U,"SAMPLER"},
            {1499262730U,"EXTENSIONSUSED"},
            {996948758U,"EXTENSIONSREQUIRED"},
            {2414822891U,"ACCESSORS"},
            {1789696170U,"ANIMATIONS"},
            {2283815739U,"ASSET"},
            {3987518648U,"BUFFERS"},
            {3031378023U,"BUFFERVIEWS"},
            {2550069799U,"CAMERAS"},
            {1881178923U,"IMAGES"},
            {1393374089U,"MATERIALS"},
            {4206430392U,"MESHES"},
            {3675200826U,"NODES"},
            {51960179U,"SCENE"},
            {1953120U,"SCENES"},
            {1702551537U,"SKINS"},
            {3346351557U,"TEXTURES"},
            {3540378101U,"MIMETYPE"},
            {2464862628U,"PBRMETALLICROUGHNESS"},
            {3405345669U,"NORMALTEXTURE"},
            {1228653941U,"OCCLUSIONTEXTURE"},
            {3330026097U,"EMISSIVETEXTURE"},
            {2025219817U,"EMISSIVEFACTOR"},
            {3200287092U,"ALPHAMODE"},
            {2113210782U,"ALPHACUTOFF"},
            {1515629509U,"DOUBLESIDED"},
            {3884336095U,"PRIMITIVES"},
            {3302258910U,"WEIGHTS"},
            {1150800334U,"CAMERA"},
            {25294226U,"CHILDREN"},
            {1891474104U,"SKIN"},
            {3232719852U,"MATRIX"},
            {1253731580U,"MESH"},
            {1306169695U,"ROTATION"},
            {594488785U,"SCALE"},
            {2352925580U,"TRANSLATION"},
            {2977188619U,"INDEX"},
            {197415955U,"TEXCOORD"},
            {1190385072U,"STRENGTH"},
            {2641395134U,"XMAG"},
            {4266306379U,"YMAG"},
            {3102316870U,"ZFAR"},
            {3547621959U,"ZNEAR"},
            {1281948832U,"BASECOLORFACTOR"},
            {2523291950U,"BASECOLORTEXTURE"},
            {3732260891U,"METALLICFACTOR"},
            {3032623030U,"ROUGHNESSFACTOR"},
            {3979861821U,"METALLICROUGHNESSTEXTURE"},
            {770814808U,"ASPECTRATIO"},
            {3573915575U,"YFOV"},
            {2183160468U,"ATTRIBUTES"},
            {3304816210U,"INDICES"},
            {2401933216U,"MATERIAL"},
            {3593007890U,"MODE"},
            {4066453633U,"TARGETS"},
            {2867472150U,"MAGFILTER"},
            {2384703633U,"MINFILTER"},
            {3322193538U,"WRAPS"},
            {3238305443U,"WRAPT"},
            {622090976U,"INVERSEBINDMATRICES"},
            {4266997486U,"SKELETON"},
            {1803683916U,"JOINTS"},
            {3612181627U,"VALUES"},
            {2000820813U,"NODE"},
            {1839435638U,"PATH"},
            {3111715480U,"SOURCE"},
            {2013849738U,"POSITION"},
            {1009949074U,"NORMAL"},
            {2576630498U,"TANGENT"},
            {1145745916U,"TEXCOORD_0"},
            {1162523535U,"TEXCOORD_1"},
            {1179301154U,"TEXCOORD_2"},
            {1196078773U,"TEXCOORD_3"},
            {3429632335U,"COLOR_0"},
            {3412854716U,"COLOR_1"},
            {3463187573U,"COLOR_2"},
            {3446409954U,"COLOR_3"},
            {3797173403U,"JOINTS_0"},
            {3780395784U,"JOINTS_1"},
            {3830728641U,"JOINTS_2"},
            {3813951022U,"JOINTS_3"},
            {596967449U,"WEIGHTS_0"},
            {580189830U,"WEIGHTS_1"},
            {563412211U,"WEIGHTS_2"},
            {546634592U,"WEIGHTS_3"},
        };
    }

    //---------------------------------------------------------------
    //---
    //--- glTFBase
    //---
    //---------------------------------------------------------------

    //--- glTFBase::StringTable
    //---------------------------------------------------------------
    glTFBase::StringTable::StringTable()
        :capacity_(0)
        ,length_(0)
        ,strings_(CPPGLTF_NULL)
    {
    }

    glTFBase::StringTable::~StringTable()
    {
        CPPGLTF_FREE(strings_);
        strings_ = CPPGLTF_NULL;
        length_ = 0;
        capacity_ = 0;
    }

    void glTFBase::StringTable::clear()
    {
        length_ = 0;
    }

    s32 glTFBase::StringTable::add(s32 length)
    {
        s32 start = (0<length_)? length_+1 : 0;
        s32 capacity = start + length+1;
        if(capacity_<capacity){
            capacity = (capacity+0x7F) & ~0x7FU;
            Char* strings = (Char*)CPPGLTF_MALLOC(capacity*sizeof(Char));
            memcpy(strings, strings_, sizeof(Char)*capacity_);
            CPPGLTF_FREE(strings_);
            strings_ = strings;
            capacity_ = capacity;
        }
        length_ += length+1;
        strings_[length_] = '\0';
        return start;
    }

    const Char* glTFBase::StringTable::c_str(s32 start) const
    {
        return strings_+start;
    }

    Char* glTFBase::StringTable::c_str(s32 start)
    {
        return strings_+start;
    }

    //--- glTFBase
    //---------------------------------------------------------------
    glTFBase::glTFBase()
        :size_(0)
        ,capacity_(0)
        ,elements_(CPPGLTF_NULL)
    {
    }

    glTFBase::~glTFBase()
    {
        destroy();
    }

    void glTFBase::begin()
    {
        destroy();
    }

    void glTFBase::end()
    {
    }

    void glTFBase::destroy()
    {
        strings_.clear();
        key_.clear();
        if(CPPGLTF_NULL != elements_){
            for(s32 i = 0; i<size_; ++i){
                JSElement::destroy(allocator_, elements_[i]);
            }
            allocator_.dealloc(capacity_, elements_);
            size_ = 0;
            capacity_ = 0;
            elements_ = CPPGLTF_NULL;
        }
        allocator_.destroy();
    }

    void glTFBase::beginObject()
    {
        expand();
        elements_[size_].capacity_ = InitSize;
        elements_[size_].value_.type_ = JSON_Object;
        elements_[size_].value_.num_ = 0;
        elements_[size_].value_.object_ = (JSKeyValue*)allocator_.alloc(InitSize);
        ++size_;
    }

    void glTFBase::endObject(RangeStream /*object*/)
    {
        if(0<size_){
            --size_;
        }
    }

    void glTFBase::beginArray()
    {
        expand();
        elements_[size_].capacity_ = InitSize;
        elements_[size_].value_.type_ = JSON_Array;
        elements_[size_].value_.num_ = 0;
        elements_[size_].value_.array_ = (JSAny*)allocator_.alloc(InitSize);
        ++size_;
    }

    void glTFBase::endArray(RangeStream /*stream*/)
    {
        if(0<size_){
            --size_;
        }
    }

    void glTFBase::root(s32 type, RangeStream /*value*/)
    {
        if(type != JSON_Object || CPPGLTF_NULL == elements_){
            return;
        }
        parseGlTF(elements_[0].value_);
    }

    void glTFBase::value(s32 type, RangeStream v)
    {
        if(size_<=0){
            return;
        }
        CPPGLTF_ASSERT(0<size_);
        JSElement& element = elements_[size_-1];
        CPPGLTF_ASSERT(JSON_Array == element.value_.type_);
        expandArray(element);

        JSAny& val = element.value_.array_[element.value_.num_];
        ++element.value_.num_;
        switch(type){
        case JSON_Object:
            CPPGLTF_ASSERT((size_+1)<=capacity_);
            CPPGLTF_ASSERT(JSON_Object == elements_[size_].value_.type_);
            val = elements_[size_].value_;
            break;
        case JSON_Array:
            CPPGLTF_ASSERT((size_+1)<=capacity_);
            CPPGLTF_ASSERT(JSON_Array == elements_[size_].value_.type_);
            val = elements_[size_].value_;
            break;
        case JSON_String:
            val.type_ = JSON_String;
            val.num_ = v.length();
            val.string_ = strings_.add(v.length());
            v.readAsString(strings_.c_str(val.string_));
            break;
        case JSON_Integer:
            val.type_ = JSON_Integer;
            val.int_ = v.readAsInt();
            break;
        case JSON_Float:
            val.type_ = JSON_Float;
            val.float_ = v.readAsFloat();
            break;
        case JSON_True:
            val.type_ = JSON_True;
            break;
        case JSON_False:
            val.type_ = JSON_False;
            break;
        case JSON_Null:
            val.type_ = JSON_Null;
            break;
        default:
            CPPGLTF_ASSERT(false);
            break;
        }
    }

    void glTFBase::keyValue(RangeStream key, s32 type, RangeStream value)
    {
        if(size_<=0){
            return;
        }
        CPPGLTF_ASSERT(0<size_);
        JSElement& element = elements_[size_-1];
        CPPGLTF_ASSERT(JSON_Object == element.value_.type_);
        expandObject(element);

        key.readAsString(key_);
        for(s32 i=0; i<key_.length(); ++i){
            key_[i] = (Char)toupper(key_[i]);
        }

        JSKeyValue& keyValue = element.value_.object_[element.value_.num_];
        ++element.value_.num_;

        keyValue.key_ = getKeyCode(key_);
        JSAny& val = keyValue.value_;
        switch(type){
        case JSON_Object:
            CPPGLTF_ASSERT((size_+1)<=capacity_);
            CPPGLTF_ASSERT(JSON_Object == elements_[size_].value_.type_);
            val = elements_[size_].value_;
            break;
        case JSON_Array:
            CPPGLTF_ASSERT((size_+1)<=capacity_);
            CPPGLTF_ASSERT(JSON_Array == elements_[size_].value_.type_);
            val = elements_[size_].value_;
            break;
        case JSON_String:
            val.type_ = JSON_String;
            val.num_ = value.length();
            val.string_ = strings_.add(value.length());
            value.readAsString(strings_.c_str(val.string_));
            break;
        case JSON_Integer:
            val.type_ = JSON_Integer;
            val.int_ = value.readAsInt();
            break;
        case JSON_Float:
            val.type_ = JSON_Float;
            val.float_ = value.readAsFloat();
            break;
        case JSON_True:
            val.type_ = JSON_True;
            break;
        case JSON_False:
            val.type_ = JSON_False;
            break;
        case JSON_Null:
            val.type_ = JSON_Null;
            break;
        default:
            CPPGLTF_ASSERT(false);
            break;
        }
    }
#ifdef _DEBUG
    void glTFBase::onError(s32 line, s32 charCount)
    {
        printf("error around line %d %d\n", line, charCount);
    }
#else
    void glTFBase::onError(s32 /*line*/, s32 /*charCount*/)
    {
    }
#endif


    namespace
    {
        u32 hash_FNV1a(const u8* v, u32 count)
        {
            u32 hash = 2166136261U;

            for(u32 i=0; i<count; ++i){
                hash ^= v[i];
                hash *= 16777619U;
            }
            return hash;
        }

        s32 getNextSize(s32 size)
        {
            if(size<=Allocator::MaxNum){
                s32 index = 2;
                for(;Allocator::SmaleSizes[index]<size;++index){
                }
                return Allocator::SmaleSizes[index];
            } else{
                return size<<1;
            }
        }
    }
    void glTFBase::expand()
    {
        if(size_<capacity_){
            return;
        }
        s32 capacity = getNextSize(capacity_+1);
        JSElement* elements = (JSElement*)allocator_.alloc(capacity);
        memcpy(elements, elements_, sizeof(JSElement)*capacity_);
        allocator_.dealloc(capacity_, elements_);
        capacity_ = capacity;
        elements_ = elements;
    }

    void glTFBase::expandObject(JSElement& element)
    {
        if(element.value_.num_<element.capacity_){
            return;
        }
        s32 capacity = element.capacity_<<1;
        JSKeyValue* values = (JSKeyValue*)allocator_.alloc(capacity);
        memcpy(values, element.value_.object_, sizeof(JSKeyValue)*element.capacity_);
        allocator_.dealloc(element.capacity_, element.value_.object_);
        element.capacity_ = capacity;
        element.value_.object_ = values;
    }

    void glTFBase::expandArray(JSElement& element)
    {
        if(element.value_.num_<element.capacity_){
            return;
        }
        s32 capacity = element.capacity_<<1;
        JSAny* values = (JSAny*)allocator_.alloc(capacity);
        memcpy(values, element.value_.array_, sizeof(JSAny)*element.capacity_);
        allocator_.dealloc(element.capacity_, element.value_.array_);
        element.capacity_ = capacity;
        element.value_.array_ = values;
    }

    void glTFBase::getString(String& str, const JSAny& value) const
    {
        str.clear();
        if(JSON_String != value.type_){
            return;
        }
        str.assign(value.num_, strings_.c_str(value.string_));
    }

    bool glTFBase::getBoolean(const JSAny& value) const
    {
        CPPGLTF_ASSERT(JSON_True==value.type_ || JSON_False==value.type_);
        return JSON_True == value.type_? true : false;
    }

    f32 glTFBase::getNumber(const JSAny& value) const
    {
        CPPGLTF_ASSERT(JSON_Integer==value.type_ || JSON_Float==value.type_);
        return JSON_Integer == value.type_? static_cast<f32>(value.int_) : value.float_;
    }

    void glTFBase::getNumbers(s32 num, Number* x, const JSAny& value) const
    {
        CPPGLTF_ASSERT(JSON_Array == value.type_);
        CPPGLTF_ASSERT(value.num_<=num);
        for(s32 i=0; i<value.num_; ++i){
            switch(value.array_[i].type_)
            {
            case JSON_Integer:
                x[i].ivalue_ = value.array_[i].int_;
                break;
            case JSON_Float:
                x[i].fvalue_ = value.array_[i].float_;
                break;
            }
        }
    }

    void glTFBase::getVec2(f32 v[2], const JSAny& value) const
    {
        CPPGLTF_ASSERT(JSON_Array==value.type_);
        CPPGLTF_ASSERT(2<=value.num_);
        for(s32 i=0; i<2; ++i){
            v[i] = getNumber(value.array_[i]);
        }
    }

    void glTFBase::getVec3(f32 v[3], const JSAny& value) const
    {
        CPPGLTF_ASSERT(JSON_Array==value.type_);
        CPPGLTF_ASSERT(3<=value.num_);
        for(s32 i=0; i<3; ++i){
            v[i] = getNumber(value.array_[i]);
        }
    }

    void glTFBase::getVec4(f32 v[4], const JSAny& value) const
    {
        CPPGLTF_ASSERT(JSON_Array==value.type_);
        CPPGLTF_ASSERT(4<=value.num_);
        for(s32 i=0; i<4; ++i){
            v[i] = getNumber(value.array_[i]);
        }
    }

    void glTFBase::getMat44(f32 m[16], const JSAny& value) const
    {
        CPPGLTF_ASSERT(JSON_Array==value.type_);
        CPPGLTF_ASSERT(16<=value.num_);
        for(s32 i=0; i<16; ++i){
            m[i] = getNumber(value.array_[i]);
        }
    }

    s32 glTFBase::getKeyCode(const String& key)
    {
        u32 hash = hash_FNV1a(reinterpret_cast<const u8*>(key.c_str()), key.length());
        for(s32 i=0; i<NumWords; ++i){
            if(hash == words[i].hash_ && 0==strcmp(key.c_str(), words[i].str_)){
                return i;
            }
        }
        return -1;
    }

    //---------------------------------------------------------------
    //---
    //--- glTF
    //---
    //---------------------------------------------------------------
    void Flags::clear()
    {
        flags_ = 0;
    }

    bool Flags::check(u32 flag) const
    {
        return flag == (flags_ & flag);
    }

    void Flags::set(u32 flag)
    {
        flags_ |= flag;
    }

    void Flags::reset(u32 flag)
    {
        flags_ &= ~(flag);
    }

    //---------------------------------------------------------------
    void Extensions::initialize()
    {
    }

    void Extras::initialize()
    {
    }

    //---------------------------------------------------------------
    void Indices::initialize()
    {
        bufferView_ = -1;
        byteOffset_ = 0;
        componentType_ = -1;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Indices::checkRequirements() const
    {
        if(bufferView_<0){
            return false;
        }
        if(componentType_<0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Values::initialize()
    {
        bufferView_ = -1;
        byteOffset_ = 0;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Values::checkRequirements() const
    {
        if(bufferView_<0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Sparse::initialize()
    {
        count_ = -1;
        indices_.initialize();
        values_.initialize();
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Sparse::checkRequirements() const
    {
        if(count_<0){
            return false;
        }
        if(!indices_.checkRequirements()){
            return false;
        }
        if(!values_.checkRequirements()){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Accessor::initialize()
    {
        flags_.clear();
        bufferView_ = -1;
        byteOffset_ = 0;
        componentType_ = -1;
        normalized_ = false;
        count_ = -1;
        type_ = -1;
        sparse_.initialize();
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Accessor::checkRequirements() const
    {
        if(componentType_<0){
            return false;
        }
        if(count_<0){
            return false;
        }
        if(type_<0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Target::initialize()
    {
        node_ = -1;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Target::checkRequirements() const
    {
        if(path_.length()<=0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Channel::initialize()
    {
        sampler_ = -1;
        target_.initialize();
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Channel::checkRequirements() const
    {
        if(sampler_<0){
            return false;
        }
        if(!target_.checkRequirements()){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void AnimationSampler::initialize()
    {
        input_ = -1;
        interpolation_ = GLTF_INTERP_LINEAR;
        output_ = -1;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean AnimationSampler::checkRequirements() const
    {
        if(input_<0){
            return false;
        }
        if(output_<0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Animation::initialize()
    {
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Animation::checkRequirements() const
    {
        if(channels_.size()<=0){
            return false;
        }
        if(samplers_.size()<=0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Asset::initialize()
    {
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Asset::checkRequirements() const
    {
        if(version_.length()<=0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Buffer::initialize()
    {
        byteLength_ = -1;
        extensions_.initialize();
        extras_.initialize();
        data_ = CPPGLTF_NULL;
    }

    boolean Buffer::checkRequirements() const
    {
        if(byteLength_<0){
            return false;
        }
        return true;
    }

namespace
{
    static const Char* Base64URL = "data:application/octet-stream;base64,";
    FILE* open(const String& uri, const String& directory, const Char* mode)
    {
        static const s32 MaxBuffer = 128;
        s32 length = uri.length() + directory.length();
        Char buffer[MaxBuffer];
        Char* buff;
        if(length<MaxBuffer){
            buff = buffer;
        }else{
            buff = (Char*)CPPGLTF_MALLOC(sizeof(Char)*(length+1));
        }
        ::memcpy(buff, directory.c_str(), directory.length());
        ::memcpy(buff+directory.length(), uri.c_str(), uri.length());
        buff[length] = '\0';

#ifdef _MSC_VER
        FILE* file = CPPGLTF_NULL;
        if(0 != fopen_s(&file, buff, mode)){
            file = CPPGLTF_NULL;
        }
#else
        FILE* file = fopen(buff, mode);
#endif
        if(buff != buffer){
            CPPGLTF_FREE(buff);
        }
        return file;
    }
}
    boolean load(Buffer& buffer, const String& directory)
    {
        static const s32 l = static_cast<s32>(::strlen(Base64URL));
        if(buffer.uri_.startWith(Base64URL)){
            s32 dl = decodeBase64(buffer.data_, buffer.byteLength_, reinterpret_cast<const s8*>(buffer.uri_.c_str()+l));
            return dl == buffer.byteLength_;
        }

        FILE* file = open(buffer.uri_, directory, "rb");
        if(CPPGLTF_NULL == file){
            return false;
        }
        size_type ret = fread(buffer.data_, buffer.byteLength_, 1, file);
        fclose(file);
        return 0<ret;
    }

    boolean save(String& uri, const Buffer& buffer, const String& directory)
    {
        if(buffer.uri_.length()<=0){
            return true;
        }

        static const s32 l = static_cast<s32>(::strlen(Base64URL));
        if(buffer.uri_.startWith(Base64URL)){
            s32 length = getLengthEncodedBase64(buffer.byteLength_);
            s8* buff = (s8*)CPPGLTF_MALLOC(length+l+1);
            ::memcpy(buff, buffer.uri_.c_str(), l);
            encodeBase64(buff+l, length, buffer.data_);
            length +=l;
            buff[length] = '\0';
            uri.assign(length, reinterpret_cast<const Char*>(buff));
            CPPGLTF_FREE(buff);
            return true;
        }

        FILE* file = open(buffer.uri_, directory, "wb");
        if(CPPGLTF_NULL == file){
            return false;
        }
        size_type ret = fwrite(buffer.data_, buffer.byteLength_, 1, file);
        fclose(file);
        uri.assign(buffer.uri_.length(), buffer.uri_.c_str());
        return 0<ret;
    }

    //---------------------------------------------------------------
    void BufferView::initialize()
    {
        buffer_ = -1;
        byteOffset_ = 0;
        byteLength_ = -1;
        byteStride_ = -1;
        target_ = -1;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean BufferView::checkRequirements() const
    {
        if(buffer_<0){
            return false;
        }
        if(byteLength_<0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Orthographic::initialize()
    {
        xmag_ = 0.0f;
        ymag_ = 0.0f;
        zfar_ = 0.0f;
        znear_ = 0.0f;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Orthographic::checkRequirements() const
    {
        if(xmag_<=0.0f){
            return false;
        }
        if(ymag_<=0.0f){
            return false;
        }
        if(zfar_<=znear_){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Perspective::initialize()
    {
        aspectRatio_ = 0.0f;
        yfov_ = 0.0f;
        zfar_ = 0.0f;
        znear_ = 0.0f;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Perspective::checkRequirements() const
    {
        if(aspectRatio_<=0.0f){
            return false;
        }
        if(yfov_<=0.0f){
            return false;
        }
        if(zfar_<=znear_){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Camera::initialize()
    {
        orthographic_.initialize();
        perspective_.initialize();
        type_ = -1;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Camera::checkRequirements() const
    {
        switch(type_)
        {
        case GLTF_CAMERA_TYPE_PERSPECTIVE:
            return perspective_.checkRequirements();
        case GLTF_CAMERA_TYPE_ORTHOGRAPHIC:
            return orthographic_.checkRequirements();
        default:
            return false;
        }
    }

    //---------------------------------------------------------------
    void Image::initialize()
    {
        bufferView_ = -1;
        extensions_.initialize();
        extras_.initialize();
    }

    //---------------------------------------------------------------
    void TextureInfo::initialize()
    {
        index_ = -1;
        texCoord_ = 0;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean TextureInfo::checkRequirements() const
    {
        if(index_<0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void NormalTextureInfo::initialize()
    {
        index_ = -1;
        texCoord_ = 0;
        scale_ = 1.0f;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean NormalTextureInfo::checkRequirements() const
    {
        if(index_<0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void OcclusionTextureInfo::initialize()
    {
        index_ = -1;
        texCoord_ = 0;
        strength_ = 1.0f;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean OcclusionTextureInfo::checkRequirements() const
    {
        if(index_<0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void PbrMetallicRoughness::initialize()
    {
        baseColorFactor_[0] = baseColorFactor_[1] = baseColorFactor_[2] = baseColorFactor_[3] = 1.0f;
        baseColorTexture_.initialize();
        metallicFactor_ = 1.0f;
        roughnessFactor_ = 1.0f;
        metallicRoughnessTexture_.initialize();
        extensions_.initialize();
        extras_.initialize();
    }

    //---------------------------------------------------------------
    void Material::initialize()
    {
        pbrMetallicRoughness_.initialize();
        normalTexture_.initialize();
        occlusionTexture_.initialize();
        emissiveTexture_.initialize();
        emissiveFactor_[0] = emissiveFactor_[1] = emissiveFactor_[2] = 0.0f;
        alphaMode_ = GLTF_ALPHAMODE_OPAQUE;
        alphaCutoff_ = 0.5f;
        doubleSided_ = false;
        extensions_.initialize();
        extras_.initialize();
    }

    //---------------------------------------------------------------
    void Attribute::initialize()
    {
        semanticType_ = -1;
        semanticIndex_ = -1;
        accessor_ = -1;
    }

    boolean Attribute::checkRequirements() const
    {
        if(semanticType_<0){
            return false;
        }
        if(semanticIndex_<0){
            return false;
        }
        if(accessor_<0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void MorphTarget::initialize()
    {
        indices_[0] = indices_[1] = indices_[2] = -1;
    }

    boolean MorphTarget::checkRequirements() const
    {
        if(indices_[0]<0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    void Primitive::initialize()
    {
        indices_ = -1;
        material_ = -1;
        mode_ = GLTF_PRIMITIVE_TRIANGLES;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Primitive::checkRequirements() const
    {
        if(attributes_.size()<=0){
            return false;
        }
        for(s32 i=0; i<attributes_.size(); ++i){
            if(!attributes_[i].checkRequirements()){
                return false;
            }
        }
        return true;
    }

    //---------------------------------------------------------------
    void Mesh::initialize()
    {
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Mesh::checkRequirements() const
    {
        if(primitives_.size()<=0){
            return false;
        }
        for(s32 i=0; i<primitives_.size(); ++i){
            if(!primitives_[i].checkRequirements()){
                return false;
            }
        }
        return true;
    }

    //---------------------------------------------------------------
    void Node::initialize()
    {
        flags_.clear();
        camera_ = -1;
        skin_ = -1;
        matrix_[0] = 1.0f; matrix_[1] = 0.0f; matrix_[2] = 0.0f; matrix_[3] = 0.0f;
        matrix_[4] = 0.0f; matrix_[5] = 1.0f; matrix_[6] = 0.0f; matrix_[7] = 0.0f;
        matrix_[8] = 0.0f; matrix_[9] = 0.0f; matrix_[10] = 1.0f; matrix_[11] = 0.0f;
        matrix_[12] = 0.0f; matrix_[13] = 0.0f; matrix_[14] = 0.0f; matrix_[15] = 1.0f;
        mesh_ = -1;
        rotation_[0] = 0.0f; rotation_[1] = 0.0f; rotation_[2] = 0.0f; rotation_[3] = 1.0f;
        scale_[0] = scale_[1] = scale_[2] = 1.0f;
        translation_[0] = translation_[1] = translation_[2] = 0.0f;
        extensions_.initialize();
        extras_.initialize();
    }

    //---------------------------------------------------------------
    void Sampler::initialize()
    {
        magFilter_ = GLTF_LINEAR;
        minFilter_ = GLTF_LINEAR;
        wrapS_ = GLTF_REPEAT;
        wrapT_ = GLTF_REPEAT;
        extensions_.initialize();
        extras_.initialize();
    }

    //---------------------------------------------------------------
    void Scene::initialize()
    {
        extensions_.initialize();
        extras_.initialize();
    }

    //---------------------------------------------------------------
    void Skin::initialize()
    {
        inverseBindMatrices_ = -1;
        skeleton_ = -1;
        extensions_.initialize();
        extras_.initialize();
    }

    boolean Skin::checkRequirements() const
    {
        if(joints_.size()<=0){
            return false;
        }
        for(s32 i=0; i<joints_.size(); ++i){
            if(joints_[i]<0){
                return false;
            }
        }
        return true;
    }

    //---------------------------------------------------------------
    void Texture::initialize()
    {
        sampler_ = -1;
        source_ = -1;
        extensions_.initialize();
        extras_.initialize();
    }

    //---------------------------------------------------------------
    glTF::glTF()
        :size_(0)
        ,bin_(CPPGLTF_NULL)
        ,glbSize_(0)
        ,glbBin_(CPPGLTF_NULL)
    {
    }

    glTF::~glTF()
    {
        CPPGLTF_FREE(bin_);
        CPPGLTF_FREE(glbBin_);
    }

    void glTF::initialize()
    {
        extensionsUsed_.clear();
        extensionsRequired_.clear();
        accessors_.clear();
        animations_.clear();
        asset_.initialize();
        buffers_.clear();
        bufferViews_.clear();
        cameras_.clear();
        images_.clear();
        materials_.clear();
        meshes_.clear();
        nodes_.clear();
        sortedNodes_.clear();
        samplers_.clear();
        scene_ = 0;
        scenes_.clear();
        skins_.clear();
        textures_.clear();
        extensions_.initialize();
        extras_.initialize();
    }

    void glTF::setDirectory(const Char* directory)
    {
        directory_.clear();
        if(CPPGLTF_NULL == directory){
            return;
        }
        s32 len = static_cast<s32>(::strlen(directory));
        if(len<=0){
            return;
        }
        directory_.assign(len, directory);
        if('/' != directory[len-1]){
            directory_.push_back('/');
        }
    }

    void glTF::sortNodes()
    {
        sortedNodes_.clear();
        sortedNodes_.reserve(nodes_.size());

        //add root nodes
        for(s32 i=0; i<nodes_.size(); ++i){
            if(isRoot(i, nodes_)){
                SortNode n;
                n.oldId_ = i;
                n.parent_ = -1;
                n.numChildren_ = 0;
                n.childrenStart_ = -1;
                sortedNodes_.push_back(n);
            }
        }

        //By the glTF specification, a node hierarchy must be a strict tree.
        //So this algorithm works, and will end sometime.
        for(s32 i=0; i<sortedNodes_.size(); ++i){
            addChildren(i, sortedNodes_, nodes_);
        }
   }

    boolean glTF::isRoot(s32 node, const Array<Node>& nodes)
    {
        for(s32 i=0; i<nodes.size(); ++i){
            for(s32 j=0; j<nodes[i].children_.size(); ++j){
                if(node == nodes[i].children_[j]){
                    return false;
                }
            }
        }
        return true;
    }

    void glTF::addChildren(s32 parent, Array<SortNode>& dst, const Array<Node>& nodes)
    {
        const Node& parentNode = nodes[dst[parent].oldId_];
        dst[parent].childrenStart_ = dst.size();
        dst[parent].numChildren_ = parentNode.children_.size();
        //add children
        for(s32 i=0; i<parentNode.children_.size(); ++i){
            SortNode n;
            n.oldId_ = parentNode.children_[i];
            n.parent_ = parent;
            n.numChildren_ = nodes[n.oldId_].children_.size();
            n.childrenStart_ = -1;
            dst.push_back(n);
        }
    }

    boolean glTF::loadBuffers()
    {
        //Buffers
        u32 byteLength = 0;
        for(s32 i=0; i<buffers_.size(); ++i){
            if(buffers_[i].uri_.length()<=0){
                continue;
            }
            byteLength += buffers_[i].byteLength_;
        }
        allocate(byteLength);
        byteLength = 0;
        for(s32 i=0; i<buffers_.size(); ++i){
            if(0<buffers_[i].uri_.length()){
                buffers_[i].data_ = bin_+byteLength;
                if(!load(buffers_[i], directory_)){
                    return false;
                }
                byteLength += buffers_[i].byteLength_;
            }
        }
        return true;
    }

    boolean glTF::loadGLBBuffers()
    {
        u32 glbOffset = 0;
        for(s32 i=0; i<buffers_.size(); ++i){
            if(buffers_[i].uri_.length()<=0){
                buffers_[i].data_ = getGLB(glbOffset);
                glbOffset += buffers_[i].byteLength_;
            }
        }
        return true;
    }

    boolean glTF::checkRequirements() const
    {
        for(s32 i=0; i<accessors_.size(); ++i){
            if(!accessors_[i].checkRequirements()){
                return false;
            }
        }
        for(s32 i=0; i<animations_.size(); ++i){
            if(!animations_[i].checkRequirements()){
                return false;
            }
        }
        //if(!asset_.checkrequirements()){
        //    return;
        //}
        for(s32 i=0; i<buffers_.size(); ++i){
            if(!buffers_[i].checkRequirements()){
                return false;
            }
        }
        for(s32 i=0; i<bufferViews_.size(); ++i){
            if(!bufferViews_[i].checkRequirements()){
                return false;
            }
        }
        for(s32 i=0; i<cameras_.size(); ++i){
            if(!cameras_[i].checkRequirements()){
                return false;
            }
        }
        //for(s32 i=0; i<images_.size(); ++i){
        //    if(!images_[i].checkRequirements()){
        //        return false;
        //    }
        //}
        //for(s32 i=0; i<materials_.size(); ++i){
        //    if(!materials_[i].checkRequirements()){
        //        return false;
        //    }
        //}
        for(s32 i=0; i<meshes_.size(); ++i){
            if(!meshes_[i].checkRequirements()){
                return false;
            }
        }
        //for(s32 i=0; i<nodes_.size(); ++i){
        //    if(!nodes_[i].checkRequirements()){
        //        return false;
        //    }
        //}
        //for(s32 i=0; i<samplers_.size(); ++i){
        //    if(!samplers_[i].checkRequirements()){
        //        return false;
        //    }
        //}
        //for(s32 i=0; i<scenes_.size(); ++i){
        //    if(!scenes_[i].checkRequirements()){
        //        return false;
        //    }
        //}
        for(s32 i=0; i<skins_.size(); ++i){
            if(!skins_[i].checkRequirements()){
                return false;
            }
        }
        //for(s32 i=0; i<textures_.size(); ++i){
        //    if(!textures_[i].checkRequirements()){
        //        return false;
        //    }
        //}
        return true;
    }

    void glTF::allocate(u32 size)
    {
        CPPGLTF_FREE(bin_);
        size_ = size;
        bin_ = (u8*)CPPGLTF_MALLOC(size_);
    }

    u32 glTF::size() const
    {
        return size_;
    }

    const u8* glTF::getBin(u32 offset) const
    {
        CPPGLTF_ASSERT(offset<size_);
        return bin_+offset;
    }

    u8* glTF::getBin(u32 offset)
    {
        CPPGLTF_ASSERT(offset<size_);
        return bin_+offset;
    }

    void glTF::allocateGLB(u32 size)
    {
        CPPGLTF_FREE(glbBin_);
        glbSize_ = size;
        glbBin_ = (u8*)CPPGLTF_MALLOC(glbSize_);
    }

    u32 glTF::sizeGLB() const
    {
        return glbSize_;
    }

    const u8* glTF::getGLB(u32 offset) const
    {
        CPPGLTF_ASSERT(offset<glbSize_);
        return glbBin_+offset;
    }

    u8* glTF::getGLB(u32 offset)
    {
        CPPGLTF_ASSERT(offset<glbSize_);
        return glbBin_+offset;
    }

    //---------------------------------------------------------------
    //---
    //--- glTFHandler
    //---
    //---------------------------------------------------------------
    glTFHandler::glTFHandler()
    {
    }

    glTFHandler::glTFHandler(const Char* directory)
    {
        gltf_.setDirectory(directory);
    }

    glTFHandler::~glTFHandler()
    {
    }

    void glTFHandler::parseGlTF(const JSAny& glTF)
    {
        gltf_.initialize();
        for(s32 i=0; i<glTF.num_; ++i){
            JSKeyValue& kv = glTF.object_[i];
            switch(kv.key_){
            case GLTF_EXTENSIONSUSED:
                expand(gltf_.extensionsUsed_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    getString(gltf_.extensionsUsed_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_EXTENSIONSREQUIRED:
                expand(gltf_.extensionsRequired_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    getString(gltf_.extensionsRequired_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_ACCESSORS:
                expand(gltf_.accessors_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseAccessor(gltf_.accessors_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_ANIMATIONS:
                expand(gltf_.animations_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseAnimation(gltf_.animations_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_ASSET:
                parseAsset(gltf_.asset_, kv.value_);
                break;
            case GLTF_BUFFERS:
                expand(gltf_.buffers_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseBuffer(gltf_.buffers_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_BUFFERVIEWS:
                expand(gltf_.bufferViews_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseBufferView(gltf_.bufferViews_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_CAMERAS:
                expand(gltf_.cameras_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseCamera(gltf_.cameras_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_IMAGES:
                expand(gltf_.images_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseImage(gltf_.images_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_MATERIALS:
                expand(gltf_.materials_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseMaterial(gltf_.materials_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_MESHES:
                expand(gltf_.meshes_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseMesh(gltf_.meshes_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_NODES:
                expand(gltf_.nodes_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseNode(gltf_.nodes_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_SAMPLERS:
                expand(gltf_.samplers_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseSampler(gltf_.samplers_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_SCENE:
                gltf_.scene_ = kv.value_.int_;
                break;
            case GLTF_SCENES:
                expand(gltf_.scenes_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseScene(gltf_.scenes_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_SKINS:
                expand(gltf_.skins_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseSkin(gltf_.skins_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_TEXTURES:
                expand(gltf_.textures_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseTexture(gltf_.textures_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(gltf_.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(gltf_.extras_, kv.value_);
                break;
            }
        }

        //Topological sorting nodes
        gltf_.sortNodes();
        gltf_.loadBuffers();
    }

    void glTFHandler::parseAccessor(Accessor& accessor, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }

        accessor.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_BUFFERVIEW:
                accessor.bufferView_ = kv.value_.int_;
                break;
            case GLTF_BYTEOFFSET:
                accessor.byteOffset_ = kv.value_.int_;
                break;
            case GLTF_COMPONENTTYPE:
                accessor.componentType_ = kv.value_.int_;
                break;
            case GLTF_NORMALIZED:
                accessor.normalized_ = getBoolean(kv.value_);
                break;
            case GLTF_COUNT:
                accessor.count_ = kv.value_.int_;
                break;
            case GLTF_TYPE:
                getString(key_, kv.value_);
                if(GLTF_TYPE_NAME_SCALAR == key_){
                    accessor.type_ = GLTF_TYPE_SCALAR;

                }else if(GLTF_TYPE_NAME_VEC2 == key_){
                    accessor.type_ = GLTF_TYPE_VEC2;

                }else if(GLTF_TYPE_NAME_VEC3 == key_){
                    accessor.type_ = GLTF_TYPE_VEC3;

                }else if(GLTF_TYPE_NAME_VEC4 == key_){
                    accessor.type_ = GLTF_TYPE_VEC4;

                }else if(GLTF_TYPE_NAME_MAT2 == key_){
                    accessor.type_ = GLTF_TYPE_MAT2;

                }else if(GLTF_TYPE_NAME_MAT3 == key_){
                    accessor.type_ = GLTF_TYPE_MAT3;

                }else if(GLTF_TYPE_NAME_MAT4 == key_){
                    accessor.type_ = GLTF_TYPE_MAT4;
                }
                break;
            case GLTF_MAX:
                accessor.flags_.set(Accessor::Flag_Max);
                getNumbers(16, accessor.max_, kv.value_);
                break;
            case GLTF_MIN:
                accessor.flags_.set(Accessor::Flag_Min);
                getNumbers(16, accessor.min_, kv.value_);
                break;
            case GLTF_SPARSE:
                parseSparse(accessor.sparse_, kv.value_);
                break;
            case GLTF_NAME:
                getString(accessor.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(accessor.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(accessor.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseSparse(Sparse& sparse, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }

        sparse.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_COUNT:
                sparse.count_ = kv.value_.int_;
                break;
            case GLTF_INDICES:
                parseIndices(sparse.indices_, kv.value_);
                break;
            case GLTF_VALUES:
                parseValues(sparse.values_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(sparse.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(sparse.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseIndices(Indices& indices, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        indices.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_BUFFERVIEW:
                indices.bufferView_ = kv.value_.int_;
                break;
            case GLTF_BYTEOFFSET:
                indices.byteOffset_ = kv.value_.int_;
                break;
            case GLTF_COMPONENTTYPE:
                indices.componentType_ = kv.value_.int_;
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(indices.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(indices.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseValues(Values& values, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        values.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_BUFFERVIEW:
                values.bufferView_ = kv.value_.int_;
                break;
            case GLTF_BYTEOFFSET:
                values.byteOffset_ = kv.value_.int_;
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(values.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(values.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseAnimation(Animation& animation, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }

        animation.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_CHANNELS:
                expand(animation.channels_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseChannel(animation.channels_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_SAMPLERS:
                expand(animation.samplers_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseAnimationSampler(animation.samplers_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_NAME:
                getString(animation.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(animation.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(animation.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseChannel(Channel& channel, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }

        channel.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_SAMPLER:
                channel.sampler_ = kv.value_.int_;
                break;
            case GLTF_TARGET:
                parseTarget(channel.target_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(channel.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(channel.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseTarget(Target& target, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }

        target.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_NODE:
                target.node_ = kv.value_.int_;
                break;
            case GLTF_PATH:
                getString(target.path_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(target.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(target.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseAnimationSampler(AnimationSampler& sampler, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }

        sampler.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_INPUT:
                sampler.input_ = kv.value_.int_;
                break;
            case GLTF_INTERPOLATION:
                getString(key_, kv.value_);
                if(GLTF_INTERP_NAME_LINEAR == key_){
                    sampler.interpolation_ = GLTF_INTERP_LINEAR;
                }else if(GLTF_INTERP_NAME_STEP == key_){
                    sampler.interpolation_ = GLTF_INTERP_STEP;
                }else if(GLTF_INTERP_NAME_CATMULLROMSPLINE == key_){
                    sampler.interpolation_ = GLTF_INTERP_CATMULLROMSPLINE;
                }else if(GLTF_INTERP_NAME_CUBICSPLINE == key_){
                    sampler.interpolation_ = GLTF_INTERP_CUBICSPLINE;
                }
                break;
            case GLTF_OUTPUT:
                sampler.output_ = kv.value_.int_;
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(sampler.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(sampler.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseAsset(Asset& asset, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        asset.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_COPYRIGHT:
                getString(asset.copyright_, kv.value_);
                break;
            case GLTF_GENERATOR:
                getString(asset.generator_, kv.value_);
                break;
            case GLTF_VERSION:
                getString(asset.version_, kv.value_);
                break;
            case GLTF_MINVERSION:
                getString(asset.minVersion_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(asset.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(asset.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseBuffer(Buffer& buffer, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        buffer.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_URI:
                getString(buffer.uri_, kv.value_);
                break;
            case GLTF_BYTELENGTH:
                buffer.byteLength_ = kv.value_.int_;
                break;
            case GLTF_NAME:
                getString(buffer.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(buffer.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(buffer.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseBufferView(BufferView& bufferView, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        bufferView.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_BUFFER:
                bufferView.buffer_ = kv.value_.int_;
                break;
            case GLTF_BYTEOFFSET:
                bufferView.byteOffset_ = kv.value_.int_;
                break;
            case GLTF_BYTELENGTH:
                bufferView.byteLength_ = kv.value_.int_;
                break;
            case GLTF_BYTESTRIDE:
                bufferView.byteStride_ = kv.value_.int_;
                break;
            case GLTF_TARGET:
                bufferView.target_ = kv.value_.int_;
                break;
            case GLTF_NAME:
                getString(bufferView.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(bufferView.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(bufferView.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseCamera(Camera& camera, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        camera.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_ORTHOGRAPHIC:
                parseOrthographic(camera.orthographic_, value);
                break;
            case GLTF_PERSPECTIVE:
                parsePerspective(camera.perspective_, value);
                break;
            case GLTF_TYPE:
                getString(key_, value);
                if(GLTF_CAMERA_TYPE_NAME_PERSPECTIVE == key_){
                    camera.type_ = GLTF_CAMERA_TYPE_PERSPECTIVE;
                }else if(GLTF_CAMERA_TYPE_NAME_ORTHOGRAPHIC == key_){
                    camera.type_ = GLTF_CAMERA_TYPE_ORTHOGRAPHIC;
                }
                break;
            case GLTF_NAME:
                getString(camera.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(camera.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(camera.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseOrthographic(Orthographic& orthographic, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        orthographic.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_XMAG:
                orthographic.xmag_ = getNumber(kv.value_);
                break;
            case GLTF_YMAG:
                orthographic.ymag_ = getNumber(kv.value_);
                break;
            case GLTF_ZFAR:
                orthographic.zfar_ = getNumber(kv.value_);
                break;
            case GLTF_ZNEAR:
                orthographic.znear_ = getNumber(kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(orthographic.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(orthographic.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parsePerspective(Perspective& perspective, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        perspective.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_ASPECTRATIO:
                perspective.aspectRatio_ = getNumber(kv.value_);
                break;
            case GLTF_YFOV:
                perspective.yfov_ = getNumber(kv.value_);
                break;
            case GLTF_ZFAR:
                perspective.zfar_ = getNumber(kv.value_);
                break;
            case GLTF_ZNEAR:
                perspective.znear_ = getNumber(kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(perspective.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(perspective.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseImage(Image& image, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        image.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_URI:
                getString(image.uri_, kv.value_);
                break;
            case GLTF_MIMETYPE:
                getString(image.mimeType_, kv.value_);
                break;
            case GLTF_BUFFERVIEW:
                image.bufferView_ = kv.value_.int_;
                break;
            case GLTF_NAME:
                getString(image.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(image.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(image.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseMaterial(Material& material, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        material.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_NAME:
                getString(material.name_, kv.value_);
                break;
            case GLTF_PBRMETALLICROUGHNESS:
                parsePbrMetallicRoughness(material.pbrMetallicRoughness_, kv.value_);
                break;
            case GLTF_NORMALTEXTURE:
                parseNormalTexture(material.normalTexture_, kv.value_);
                break;
            case GLTF_OCCLUSIONTEXTURE:
                parseOcclusionTexture(material.occlusionTexture_, kv.value_);
                break;
            case GLTF_EMISSIVETEXTURE:
                parseTextureInfo(material.emissiveTexture_, kv.value_);
                break;
            case GLTF_EMISSIVEFACTOR:
                getVec3(material.emissiveFactor_, kv.value_);
                break;
            case GLTF_ALPHAMODE:
                getString(key_, kv.value_);
                if(GLTF_ALPHAMODE_NAME_OPAQUE == key_){
                    material.alphaMode_ = GLTF_ALPHAMODE_OPAQUE;
                }else if(GLTF_ALPHAMODE_NAME_MASK == key_){
                    material.alphaMode_ = GLTF_ALPHAMODE_MASK;
                }else if(GLTF_ALPHAMODE_NAME_BLEND == key_){
                    material.alphaMode_ = GLTF_ALPHAMODE_BLEND;
                }
                break;
            case GLTF_ALPHACUTOFF:
                material.alphaCutoff_ = getNumber(kv.value_);
                break;
            case GLTF_DOUBLESIDED:
                material.doubleSided_ = getBoolean(kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(material.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(material.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parsePbrMetallicRoughness(PbrMetallicRoughness& pbrMetallicRoughness, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        pbrMetallicRoughness.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_BASECOLORFACTOR:
                getVec4(pbrMetallicRoughness.baseColorFactor_, kv.value_);
                break;
            case GLTF_BASECOLORTEXTURE:
                parseTextureInfo(pbrMetallicRoughness.baseColorTexture_, kv.value_);
                break;
            case GLTF_METALLICFACTOR:
                pbrMetallicRoughness.metallicFactor_ = getNumber(kv.value_);
                break;
            case GLTF_ROUGHNESSFACTOR:
                pbrMetallicRoughness.roughnessFactor_ = getNumber(kv.value_);
                break;
            case GLTF_METALLICROUGHNESSTEXTURE:
                parseTextureInfo(pbrMetallicRoughness.metallicRoughnessTexture_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(pbrMetallicRoughness.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(pbrMetallicRoughness.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseNormalTexture(NormalTextureInfo& normal, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        normal.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_INDEX:
                normal.index_ = kv.value_.int_;
                break;
            case GLTF_TEXCOORD:
                normal.texCoord_ = kv.value_.int_;
                break;
            case GLTF_SCALE:
                normal.scale_ = getNumber(kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(normal.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(normal.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseOcclusionTexture(OcclusionTextureInfo& occlusion, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        occlusion.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_INDEX:
                occlusion.index_ = kv.value_.int_;
                break;
            case GLTF_TEXCOORD:
                occlusion.texCoord_ = kv.value_.int_;
                break;
            case GLTF_STRENGTH:
                occlusion.strength_ = getNumber(kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(occlusion.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(occlusion.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseMesh(Mesh& mesh, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        mesh.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_PRIMITIVES:
                expand(mesh.primitives_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parsePrimitive(mesh.primitives_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_WEIGHTS:
                expand(mesh.weights_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    mesh.weights_[j] = getNumber(kv.value_.array_[j]);
                }
                break;
            case GLTF_NAME:
                getString(mesh.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(mesh.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(mesh.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parsePrimitive(Primitive& primitive, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        primitive.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_ATTRIBUTES:
                expandObjects(primitive.attributes_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseAttribute(primitive.attributes_[j], kv.value_.object_[j]);
                }
                break;
            case GLTF_INDICES:
                primitive.indices_ = kv.value_.int_;
                break;
            case GLTF_MATERIAL:
                primitive.material_ = kv.value_.int_;
                break;
            case GLTF_MODE:
                primitive.mode_ = kv.value_.int_;
                break;
            case GLTF_TARGETS:
                expand(primitive.targets_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    parseMorphTarget(primitive.targets_[j], kv.value_.array_[j]);
                }
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(primitive.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(primitive.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseAttribute(Attribute& attribute, const JSKeyValue& keyValue)
    {
        attribute.initialize();
        attribute.accessor_ = static_cast<s16>(keyValue.value_.int_);
        switch(keyValue.key_)
        {
        case GLTF_POSITION:
            attribute.semanticType_ = GLTF_ATTRIBUTE_POSITION;
            attribute.semanticIndex_ = 0;
            break;
        case GLTF_NORMAL:
            attribute.semanticType_ = GLTF_ATTRIBUTE_NORMAL;
            attribute.semanticIndex_ = 0;
            break;
        case GLTF_TANGENT:
            attribute.semanticType_ = GLTF_ATTRIBUTE_TANGENT;
            attribute.semanticIndex_ = 0;
            break;
        case GLTF_TEXCOORD_0:
            attribute.semanticType_ = GLTF_ATTRIBUTE_TEXCOORD;
            attribute.semanticIndex_ = 0;
            break;
        case GLTF_TEXCOORD_1:
            attribute.semanticType_ = GLTF_ATTRIBUTE_TEXCOORD;
            attribute.semanticIndex_ = 1;
            break;
        case GLTF_TEXCOORD_2:
            attribute.semanticType_ = GLTF_ATTRIBUTE_TEXCOORD;
            attribute.semanticIndex_ = 2;
            break;
        case GLTF_TEXCOORD_3:
            attribute.semanticType_ = GLTF_ATTRIBUTE_TEXCOORD;
            attribute.semanticIndex_ = 3;
            break;
        case GLTF_COLOR_0:
            attribute.semanticType_ = GLTF_ATTRIBUTE_COLOR;
            attribute.semanticIndex_ = 0;
            break;
        case GLTF_COLOR_1:
            attribute.semanticType_ = GLTF_ATTRIBUTE_COLOR;
            attribute.semanticIndex_ = 1;
            break;
        case GLTF_COLOR_2:
            attribute.semanticType_ = GLTF_ATTRIBUTE_COLOR;
            attribute.semanticIndex_ = 2;
            break;
        case GLTF_COLOR_3:
            attribute.semanticType_ = GLTF_ATTRIBUTE_COLOR;
            attribute.semanticIndex_ = 3;
            break;
        case GLTF_JOINTS_0:
            attribute.semanticType_ = GLTF_ATTRIBUTE_JOINTS;
            attribute.semanticIndex_ = 0;
            break;
        case GLTF_JOINTS_1:
            attribute.semanticType_ = GLTF_ATTRIBUTE_JOINTS;
            attribute.semanticIndex_ = 1;
            break;
        case GLTF_JOINTS_2:
            attribute.semanticType_ = GLTF_ATTRIBUTE_JOINTS;
            attribute.semanticIndex_ = 2;
            break;
        case GLTF_JOINTS_3:
            attribute.semanticType_ = GLTF_ATTRIBUTE_JOINTS;
            attribute.semanticIndex_ = 3;
            break;
        case GLTF_WEIGHTS_0:
            attribute.semanticType_ = GLTF_ATTRIBUTE_WEIGHTS;
            attribute.semanticIndex_ = 0;
            break;
        case GLTF_WEIGHTS_1:
            attribute.semanticType_ = GLTF_ATTRIBUTE_WEIGHTS;
            attribute.semanticIndex_ = 1;
            break;
        case GLTF_WEIGHTS_2:
            attribute.semanticType_ = GLTF_ATTRIBUTE_WEIGHTS;
            attribute.semanticIndex_ = 2;
            break;
        case GLTF_WEIGHTS_3:
            attribute.semanticType_ = GLTF_ATTRIBUTE_WEIGHTS;
            attribute.semanticIndex_ = 3;
            break;
        }
    }

    void glTFHandler::parseMorphTarget(MorphTarget& morphTarget, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        morphTarget.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_POSITION:
                morphTarget.indices_[0] = kv.value_.int_;
                break;
            case GLTF_NORMAL:
                morphTarget.indices_[1] = kv.value_.int_;
                break;
            case GLTF_TANGENT:
                morphTarget.indices_[2] = kv.value_.int_;
                break;
            }
        }
    }

    void glTFHandler::parseNode(Node& node, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        node.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_CAMERA:
                node.camera_ = kv.value_.int_;
                break;
            case GLTF_CHILDREN:
                expand(node.children_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    node.children_[j] = kv.value_.array_[j].int_;
                }
                break;
            case GLTF_SKIN:
                node.skin_ = kv.value_.int_;
                break;
            case GLTF_MATRIX:
                node.flags_.set(Node::Flag_Matrix);
                getMat44(node.matrix_, kv.value_);
                break;
            case GLTF_MESH:
                node.mesh_ = kv.value_.int_;
                break;
            case GLTF_ROTATION:
                node.flags_.set(Node::Flag_Rotation);
                getVec4(node.rotation_, kv.value_);
                break;
            case GLTF_SCALE:
                node.flags_.set(Node::Flag_Scale);
                getVec3(node.scale_, kv.value_);
                break;
            case GLTF_TRANSLATION:
                node.flags_.set(Node::Flag_Translation);
                getVec3(node.translation_, kv.value_);
                break;
            case GLTF_WEIGHTS:
                expand(node.weights_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    node.weights_[j] = getNumber(kv.value_.array_[j]);
                }
                break;
            case GLTF_NAME:
                getString(node.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(node.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(node.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseSampler(Sampler& sampler, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        sampler.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_MAGFILTER:
                sampler.magFilter_ = kv.value_.int_;
                break;
            case GLTF_MINFILTER:
                sampler.minFilter_ = kv.value_.int_;
                break;
            case GLTF_WRAPS:
                sampler.wrapS_ = kv.value_.int_;
                break;
            case GLTF_WRAPT:
                sampler.wrapT_ = kv.value_.int_;
                break;
            case GLTF_NAME:
                getString(sampler.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(sampler.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(sampler.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseScene(Scene& scene, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        scene.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_NODES:
                expand(scene.nodes_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    scene.nodes_[j] = kv.value_.array_[j].int_;
                }
                break;
            case GLTF_NAME:
                getString(scene.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(scene.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(scene.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseSkin(Skin& skin, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        skin.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_INVERSEBINDMATRICES:
                skin.inverseBindMatrices_ = kv.value_.int_;
                break;
            case GLTF_SKELETON:
                skin.skeleton_ = kv.value_.int_;
                break;
            case GLTF_JOINTS:
                expand(skin.joints_, kv.value_);
                for(s32 j=0; j<kv.value_.num_; ++j){
                    skin.joints_[j] = kv.value_.array_[j].int_;
                }
                break;
            case GLTF_NAME:
                getString(skin.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(skin.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(skin.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseTexture(Texture& texture, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        texture.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_SAMPLER:
                texture.sampler_ = kv.value_.int_;
                break;
            case GLTF_SOURCE:
                texture.source_ = kv.value_.int_;
                break;
            case GLTF_NAME:
                getString(texture.name_, kv.value_);
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(texture.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(texture.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseTextureInfo(TextureInfo& texture, const JSAny& value)
    {
        if(value.type_ != JSON_Object){
            return;
        }
        texture.initialize();
        for(s32 i=0; i<value.num_; ++i){
            JSKeyValue& kv = value.object_[i];
            switch(kv.key_){
            case GLTF_INDEX:
                texture.index_ = kv.value_.int_;
                break;
            case GLTF_TEXCOORD:
                texture.texCoord_ = kv.value_.int_;
                break;
            case GLTF_EXTENSIONS:
                parseExtensions(texture.extensions_, kv.value_);
                break;
            case GLTF_EXTRAS:
                parseExtras(texture.extras_, kv.value_);
                break;
            }
        }
    }

    void glTFHandler::parseExtensions(Extensions& /*extensions*/, const JSAny& /*value*/)
    {

    }

    void glTFHandler::parseExtras(Extras& /*extras*/, const JSAny& /*value*/)
    {

    }

    //---------------------------------------------------------------
    //---
    //--- GLBEventHandler
    //---
    //---------------------------------------------------------------
    GLBEventHandler::GLBEventHandler()
    {
    }

    GLBEventHandler::~GLBEventHandler()
    {
    }

    bool GLBEventHandler::parseHeader(u32 magic, u32 version, u32 /*length*/)
    {
        if(magic != GLBReader::Magic){
            return false;
        }
        if(version != GLBReader::Version){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    //---
    //--- GLBReader
    //---
    //---------------------------------------------------------------
    GLBReader::GLBReader(IStream& istream, GLBEventHandler& handler)
        :JSONReader(istream, handler)
        ,handler_(handler)
    {
    }

    GLBReader::~GLBReader()
    {
    }

    bool GLBReader::read(s32 flags)
    {
        flags_ = flags;
        Header header;
        s32 pos;
        u32 size;
        if(istream_.read(header)<=0){
            return false;
        }
        if(!handler_.parseHeader(header.magic_, header.version_, header.length_)){
            return false;
        }

        if(!readChunkJSON(size)){
            return false;
        }
        pos = istream_.tell();
        if(!JSONReader::read(flags_)){
            return false;
        }
        istream_.seek(pos+size);
        if(!readChunkBIN(size)){
            return false;
        }
        handler_.get().loadGLBBuffers();
        return true;
    }

    bool GLBReader::readChunkJSON(u32& size)
    {
        size = 0;
        u32 type;
        s32 pos = istream_.tell();
        for(;;){
            if(istream_.read(size)<=0){
                return false;
            }
            if(size<=0){
                continue;
            }
            if(istream_.read(type)<=0){
                return false;
            }
            if(type != ChunkType_JSON){
                if(!istream_.seek(pos+size)){
                    return false;
                }
                pos = istream_.tell();
                continue;
            }
            break;
        }
        return true;
    }

    bool GLBReader::readChunkBIN(u32& size)
    {
        size = 0;
        u32 type;
        s32 pos = istream_.tell();
        for(;;){
            if(istream_.read(size)<=0){
                return false;
            }
            if(size<=0){
                continue;
            }
            if(istream_.read(type)<=0){
                return false;
            }
            if(type != ChunkType_BIN){
                if(!istream_.seek(pos+size)){
                    return false;
                }
                pos = istream_.tell();
                continue;
            }
            break;
        }
        glTF& gltf = handler_.get();
        gltf.allocateGLB(static_cast<u32>(size));
        if(istream_.read(gltf.getGLB(0), size) <= 0){
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------
    //---
    //--- glTFWriter
    //---
    //---------------------------------------------------------------
    glTFWriter::glTFWriter(OStream& ostream)
        :gltf_(CPPGLTF_NULL)
        ,indent_(0)
        ,ostream0_(ostream)
    {
        ostream1_ = &ostream0_;
    }

    glTFWriter::~glTFWriter()
    {
    }

    bool glTFWriter::write(const glTF& gltf, GLTF_FILE type, u32 flags)
    {
        flags_.flags_ = flags;
        gltf_ = &gltf;
        indent_ = 0;

        OSStream osstream;

        switch(type){
        case GLTF_FILE_GLB:
            ostream1_ = &osstream;
            break;
        default:
            break;
        }
        ostream1_->write('{');
        printLine();
        {
            Indent indent(indent_);

            //extensionsUsed
            printArray("extensionsUsed", gltf.extensionsUsed_);

            //extensionsRequired
            printArray("extensionsRequired", gltf.extensionsRequired_);

            //accessors
            printArray("accessors", gltf.accessors_);

            //animations
            printArray("animations", gltf.animations_);

            //asset
            printObjectProperty("asset", gltf.asset_);

            //buffers
            printArray("buffers", gltf.buffers_);

            //bufferViews
            printArray("bufferViews", gltf.bufferViews_);

            //cameras
            printArray("cameras", gltf.cameras_);

            //images
            printArray("images", gltf.images_);

            //materials
            printArray("materials", gltf.materials_);

            //meshes
            printArray("meshes", gltf.meshes_);

            //nodes
            printArray("nodes", gltf.nodes_);

            //samplers
            printArray("samplers", gltf.samplers_);

            //scene
            printObjectProperty("scene", gltf.scene_);

            //scenes
            printArray("scenes", gltf.scenes_);

            //skins
            printArray("skins", gltf.skins_);

            //textures
            printArray("textures", gltf.textures_);

            if(gltf.extensions_.requiredOut()){
                printObjectProperty("extensions", gltf.extensions_);

            }
            if(gltf.extras_.requiredOut()){
                printObjectProperty("extras", gltf.extras_);

            }
            replaceLastLine();
        }
        ostream1_->write('}');

        switch(type){
        case GLTF_FILE_GLB:
        {
            ostream1_ = &ostream0_;
            GLBReader::Header header;
            header.magic_ = GLBReader::Magic;
            header.version_ = GLBReader::Version;
            header.length_ = osstream.size() + gltf.sizeGLB() + sizeof(GLBReader::Chunk)*2;
            ostream1_->write(header);

            GLBReader::Chunk chunk;
            chunk.length_ = osstream.size();
            chunk.type_ = GLBReader::ChunkType_JSON;
            ostream1_->write(chunk);
            ostream1_->write(osstream.size(), osstream.buff());

            chunk.length_ = gltf.sizeGLB();
            chunk.type_ = GLBReader::ChunkType_BIN;
            ostream1_->write(chunk);
            ostream1_->write(gltf.sizeGLB(), gltf.getGLB(0));
        }
            break;
        default:
            break;
        }

        gltf_ = CPPGLTF_NULL;
        return true;
    }

    void glTFWriter::printIndent()
    {
        if(!flags_.check(Flag_Format)){
            return;
        }
        s32 indent = indent_<<2;
        for(s32 i=0; i<indent; ++i){
            ostream1_->write(' ');
        }
    }

    void glTFWriter::printLine()
    {
        if(!flags_.check(Flag_Format)){
            return;
        }
        ostream1_->write('\n');
    }

    void glTFWriter::printSeparator()
    {
        ostream1_->write(',');
    }

    void glTFWriter::printSeparatorLine()
    {
        ostream1_->write(2, ",\n");
    }

    void glTFWriter::printKeyValueSeparator()
    {
        ostream1_->write(2, ": ");
    }

    void glTFWriter::replaceLastLine()
    {
        ostream1_->replaceLast(2, "\n");
    }

    void glTFWriter::format(const Char* str, ...)
    {
        CPPGLTF_ASSERT(CPPGLTF_NULL != str);
        va_list args;
        va_start(args, str);
#if _MSC_VER
        s32 length = _vscprintf(str, args);
#else
        s32 length = vscprintf(str, args);
#endif
        static const s32 MaxBuffer = 128;
        Char buffer[MaxBuffer];
        Char* buff;
        if(length<MaxBuffer){
            buff = buffer;
        }else{
            buff = (Char*)CPPGLTF_MALLOC(sizeof(Char)*(length+1));
        }

#if defined(_WIN32) || defined(_WIN64)
        vsnprintf_s(buff, MaxBuffer, length, str, args);
#else
        ::vsnprintf(buff, length, str, args);
#endif //defined(_WIN32) || defined(_WIN64)
        ostream1_->write(length, buff);
        if(buffer != buff){
            CPPGLTF_FREE(buff);
        }
        va_end(args);
    }

    bool glTFWriter::print(const Char* str)
    {
        ostream1_->write('\"');
        ostream1_->write(str);
        ostream1_->write('\"');
        return true;
    }

    bool glTFWriter::print(const String& str)
    {
        ostream1_->write('\"');
        ostream1_->write(str.length(), str.c_str());
        ostream1_->write('\"');
        return true;
    }

    bool glTFWriter::print(s32 value)
    {
        format("%d", value);
        return true;
    }

    bool glTFWriter::print(u32 value)
    {
        format("%d", value);
        return true;
    }

    bool glTFWriter::print(f32 value)
    {
        format("%0.1f", value);
        return true;
    }

    bool glTFWriter::print(boolean value)
    {
        const Char* str = (value)? "true":"false";
        ostream1_->write(str);
        return true;
    }

    void glTFWriter::printNull()
    {
        ostream1_->write(4, "null");
    }

    void glTFWriter::beginObject()
    {
        ostream1_->write('{');
        printLine();
    }

    void glTFWriter::endObject()
    {
        printIndent();
        ostream1_->write('}');
    }

    void glTFWriter::beginArray()
    {
        ostream1_->write('[');
        printLine();
    }

    void glTFWriter::endArray()
    {
        printIndent();
        ostream1_->write(']');
    }

    void glTFWriter::printObjectProperty(const Char* key, const String& value)
    {
        printIndent();
        print(key);
        printKeyValueSeparator();
        print(value);
        printSeparatorLine();
    }

    void glTFWriter::printObjectProperty(const Char* key, const Char* value)
    {
        printIndent();
        print(key);
        printKeyValueSeparator();
        print(value);
        printSeparatorLine();
    }

    void glTFWriter::printObjectProperty(const Char* key, s32 value)
    {
        printIndent();
        print(key);
        printKeyValueSeparator();
        print(value);
        printSeparatorLine();
    }

    void glTFWriter::printObjectProperty(const Char* key, f32 value)
    {
        printIndent();
        print(key);
        printKeyValueSeparator();
        print(value);
        printSeparatorLine();
    }

    void glTFWriter::printObjectProperty(const Char* key, s32 num, const f32* value)
    {
        CPPGLTF_ASSERT(0<num);
        --num;
        printIndent();
        print(key);
        printKeyValueSeparator();
        ostream1_->write('[');
        for(s32 i=0; i<num; ++i){
            print(value[i]);
            ostream1_->write(',');
        }
        print(value[num]);
        ostream1_->write(']');
        printSeparatorLine();
    }

    void glTFWriter::printObjectProperty(const Char* key, s32 num, const s32* value)
    {
        CPPGLTF_ASSERT(0<num);
        --num;
        printIndent();
        print(key);
        printKeyValueSeparator();
        ostream1_->write('[');
        for(s32 i=0; i<num; ++i){
            print(value[i]);
            ostream1_->write(',');
        }
        print(value[num]);
        ostream1_->write(']');
        printSeparatorLine();
    }

    void glTFWriter::printObjectProperty(const Char* key, s32 num, const u32* value)
    {
        CPPGLTF_ASSERT(0<num);
        --num;
        printIndent();
        print(key);
        printKeyValueSeparator();
        ostream1_->write('[');
        for(s32 i=0; i<num; ++i){
            print(value[i]);
            ostream1_->write(',');
        }
        print(value[num]);
        ostream1_->write(']');
        printSeparatorLine();
    }

    void glTFWriter::printObjectProperty(s32 componentType, s32 type, const Char* key, const Number* value)
    {
        s32 num=0;
        switch(type){
        case GLTF_TYPE_SCALAR:
            num = 1;
            break;
        case GLTF_TYPE_VEC2:
            num = 2;
            break;
        case GLTF_TYPE_VEC3:
            num = 3;
            break;
        case GLTF_TYPE_VEC4:
        case GLTF_TYPE_MAT2:
            num = 4;
            break;
        case GLTF_TYPE_MAT3:
            num = 9;
            break;
        case GLTF_TYPE_MAT4:
            num = 16;
            break;
        }

        switch(componentType){
        case GLTF_TYPE_BYTE:
        case GLTF_TYPE_UNSIGNED_BYTE:
        case GLTF_TYPE_SHORT:
        case GLTF_TYPE_UNSIGNED_SHORT:
            printObjectProperty(key, num, reinterpret_cast<const s32*>(value));
            break;
        case GLTF_TYPE_UNSIGNED_INT:
            printObjectProperty(key, num, reinterpret_cast<const u32*>(value));
            break;
        case GLTF_TYPE_FLOAT:
            printObjectProperty(key, num, reinterpret_cast<const f32*>(value));
            break;
        }
    }

    void glTFWriter::printObjectProperty(const Char* key, boolean value)
    {
        printIndent();
        print(key);
        printKeyValueSeparator();
        print(value);
        printSeparatorLine();
    }

    bool glTFWriter::print(const Accessor& accessor)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(0<=accessor.bufferView_){
                printObjectProperty("bufferView", accessor.bufferView_);
            }
            if(0<=accessor.byteOffset_){
                printObjectProperty("byteOffset", accessor.byteOffset_);
            }
            if(accessor.componentType_<0){
                return false;
            }
            printObjectProperty("componentType", accessor.componentType_);

            if(accessor.normalized_){
                printObjectProperty("normalized", accessor.normalized_);
            }

            if(accessor.count_<0){
                return false;
            }
            printObjectProperty("count", accessor.count_);
            switch(accessor.type_){
            case GLTF_TYPE_SCALAR:
                printObjectProperty("type", GLTF_TYPE_NAME_SCALAR);
                break;
            case GLTF_TYPE_VEC2:
                printObjectProperty("type", GLTF_TYPE_NAME_VEC2);
                break;
            case GLTF_TYPE_VEC3:
                printObjectProperty("type", GLTF_TYPE_NAME_VEC3);
                break;
            case GLTF_TYPE_VEC4:
                printObjectProperty("type", GLTF_TYPE_NAME_VEC4);
                break;
            case GLTF_TYPE_MAT2:
                printObjectProperty("type", GLTF_TYPE_NAME_MAT2);
                break;
            case GLTF_TYPE_MAT3:
                printObjectProperty("type", GLTF_TYPE_NAME_MAT3);
                break;
            case GLTF_TYPE_MAT4:
                printObjectProperty("type", GLTF_TYPE_NAME_MAT4);
                break;
            default:
                return false;
            }
            printObjectProperty(accessor.componentType_, accessor.type_, "max", accessor.max_);
            printObjectProperty(accessor.componentType_, accessor.type_, "min", accessor.min_);

            if(0<=accessor.count_ && 0<=accessor.sparse_.indices_.bufferView_ && 0<=accessor.sparse_.values_.bufferView_){
                printObjectProperty("sparse", accessor.sparse_);
            }

            if(0<accessor.name_.length()){
                printObjectProperty("name", accessor.name_);
            }
            if(accessor.extensions_.requiredOut()){
                printObjectProperty("extensions", accessor.extensions_);
            }
            if(accessor.extras_.requiredOut()){
                printObjectProperty("extras", accessor.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Sparse& sparse)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(sparse.count_<=0){
                return false;
            }
            printObjectProperty("count", sparse.count_);

            if(sparse.indices_.bufferView_<0){
                return false;
            }
            printObjectProperty("indices", sparse.indices_);

            if(sparse.values_.bufferView_<0){
                return false;
            }
            printObjectProperty("values", sparse.values_);

            if(sparse.extensions_.requiredOut()){
                printObjectProperty("extensions", sparse.extensions_);
            }
            if(sparse.extras_.requiredOut()){
                printObjectProperty("extras", sparse.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Indices& indices)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(indices.bufferView_<=0){
                return false;
            }
            printObjectProperty("bufferView", indices.bufferView_);

            if(0<=indices.byteOffset_){
                printObjectProperty("byteOffset", indices.byteOffset_);
            }

            if(indices.componentType_<0){
                return false;
            }
            printObjectProperty("componentType", indices.componentType_);

            if(indices.extensions_.requiredOut()){
                printObjectProperty("extensions", indices.extensions_);
            }
            if(indices.extras_.requiredOut()){
                printObjectProperty("extras", indices.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Values& values)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(values.bufferView_<=0){
                return false;
            }
            printObjectProperty("bufferView", values.bufferView_);

            if(0<=values.byteOffset_){
                printObjectProperty("byteOffset", values.byteOffset_);
            }

            if(values.extensions_.requiredOut()){
                printObjectProperty("extensions", values.extensions_);
            }
            if(values.extras_.requiredOut()){
                printObjectProperty("extras", values.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Animation& animation)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(!printArray("channels", animation.channels_)){
                return false;
            }

            if(!printArray("samplers", animation.samplers_)){
                return false;
            }

            if(0<animation.name_.length()){
                printObjectProperty("name", animation.name_);
            }
            if(animation.extensions_.requiredOut()){
                printObjectProperty("extensions", animation.extensions_);
            }
            if(animation.extras_.requiredOut()){
                printObjectProperty("extras", animation.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Channel& channel)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(channel.sampler_<0){
                return false;
            }
            printObjectProperty("sampler", channel.sampler_);

            if(channel.target_.path_.length()<=0){
                return false;
            }
            printObjectProperty("target", channel.target_);

            if(channel.extensions_.requiredOut()){
                printObjectProperty("extensions", channel.extensions_);
            }
            if(channel.extras_.requiredOut()){
                printObjectProperty("extras", channel.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const AnimationSampler& animSampler)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(animSampler.input_<0){
                return false;
            }
            switch(animSampler.interpolation_){
            case GLTF_INTERP_LINEAR:
                printObjectProperty("interpolation", GLTF_INTERP_NAME_LINEAR);
                break;
            case GLTF_INTERP_STEP:
                printObjectProperty("interpolation", GLTF_INTERP_NAME_STEP);
                break;
            case GLTF_INTERP_CATMULLROMSPLINE:
                printObjectProperty("interpolation", GLTF_INTERP_NAME_CATMULLROMSPLINE);
                break;
            case GLTF_INTERP_CUBICSPLINE:
                printObjectProperty("interpolation", GLTF_INTERP_NAME_CUBICSPLINE);
                break;
            default:
                break;
            }
            if(animSampler.output_<0){
                return false;
            }
            printObjectProperty("output", animSampler.output_);

            if(animSampler.extensions_.requiredOut()){
                printObjectProperty("extensions", animSampler.extensions_);
            }
            if(animSampler.extras_.requiredOut()){
                printObjectProperty("extras", animSampler.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Target& target)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(0<target.node_){
                printObjectProperty("node", target.node_);
            }
            if(target.path_.length()<=0){
                return false;
            }

            if(target.extensions_.requiredOut()){
                printObjectProperty("extensions", target.extensions_);
            }
            if(target.extras_.requiredOut()){
                printObjectProperty("extras", target.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Asset& asset)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(0<asset.copyright_.length()){
                printObjectProperty("copyright", asset.copyright_);
            }
            if(0<asset.generator_.length()){
                printObjectProperty("generator", asset.generator_);
            }
            if(asset.version_.length()<=0){
                return false;
            }
            printObjectProperty("version", asset.version_);
            if(0<asset.minVersion_.length()){
                printObjectProperty("minVersion", asset.minVersion_);
            }
            if(asset.extensions_.requiredOut()){
                printObjectProperty("extensions", asset.extensions_);
            }
            if(asset.extras_.requiredOut()){
                printObjectProperty("extras", asset.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Buffer& buffer)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(0<buffer.uri_.length()){
                String uri;
                save(uri, buffer, gltf_->directory_);
                printObjectProperty("uri", uri);
            }

            if(buffer.byteLength_<0){
                return false;
            }
            printObjectProperty("byteLength", buffer.byteLength_);

            if(0<buffer.name_.length()){
                printObjectProperty("name", buffer.name_);
            }
            
            if(buffer.extensions_.requiredOut()){
                printObjectProperty("extensions", buffer.extensions_);
            }
            if(buffer.extras_.requiredOut()){
                printObjectProperty("extras", buffer.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const BufferView& bufferView)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(bufferView.buffer_<0){
                return false;
            }
            printObjectProperty("buffer", bufferView.buffer_);

            if(0<=bufferView.byteOffset_){
                printObjectProperty("byteOffset", bufferView.byteOffset_);
            }

            if(bufferView.byteLength_<0){
                return false;
            }
            printObjectProperty("byteLength", bufferView.byteLength_);

            if(0<=bufferView.byteStride_){
                printObjectProperty("byteStride", bufferView.byteStride_);
            }

            if(0<=bufferView.target_){
                printObjectProperty("target", bufferView.target_);
            }

            if(0<bufferView.name_.length()){
                printObjectProperty("name", bufferView.name_);
            }

            if(bufferView.extensions_.requiredOut()){
                printObjectProperty("extensions", bufferView.extensions_);
            }
            if(bufferView.extras_.requiredOut()){
                printObjectProperty("extras", bufferView.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Camera& camera)
    {
        beginObject();
        {
            Indent indent(indent_);

            switch(camera.type_)
            {
            case GLTF_CAMERA_TYPE_PERSPECTIVE:
                printObjectProperty("type", GLTF_CAMERA_TYPE_NAME_PERSPECTIVE);
                printObjectProperty("perspective", camera.perspective_);
                break;
            case GLTF_CAMERA_TYPE_ORTHOGRAPHIC:
                printObjectProperty("type", GLTF_CAMERA_TYPE_NAME_ORTHOGRAPHIC);
                printObjectProperty("orthographic", camera.orthographic_);
                break;
            default:
                return false;
            }
            if(0<camera.name_.length()){
                printObjectProperty("name", camera.name_);
            }

            if(camera.extensions_.requiredOut()){
                printObjectProperty("extensions", camera.extensions_);
            }
            if(camera.extras_.requiredOut()){
                printObjectProperty("extras", camera.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Orthographic& orthographic)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(orthographic.xmag_<=0.0f){
                return false;
            }
            printObjectProperty("xmag", orthographic.xmag_);

            if(orthographic.ymag_<=0.0f){
                return false;
            }
            printObjectProperty("ymag", orthographic.ymag_);

            if(orthographic.zfar_<0.0f){
                return false;
            }
            printObjectProperty("zfar", orthographic.zfar_);

            if(orthographic.znear_<0.0f){
                return false;
            }
            printObjectProperty("znear", orthographic.znear_);

            if(orthographic.extensions_.requiredOut()){
                printObjectProperty("extensions", orthographic.extensions_);
            }
            if(orthographic.extras_.requiredOut()){
                printObjectProperty("extras", orthographic.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Perspective& perspective)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(0.0f<perspective.aspectRatio_){
                printObjectProperty("aspectRatio", perspective.aspectRatio_);
            }

            if(perspective.yfov_<=0.0f){
                return false;
            }
            printObjectProperty("yfov", perspective.yfov_);

            if(0.0f<perspective.zfar_){
                printObjectProperty("zfar", perspective.zfar_);
            }

            if(0.0f<=perspective.znear_){
                printObjectProperty("znear", perspective.znear_);
            }

            if(perspective.extensions_.requiredOut()){
                printObjectProperty("extensions", perspective.extensions_);
            }
            if(perspective.extras_.requiredOut()){
                printObjectProperty("extras", perspective.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Extensions& /*extensions*/)
    {
        return true;
    }

    bool glTFWriter::print(const Extras& /*extras*/)
    {
        return true;
    }

    bool glTFWriter::print(const Image& image)
    {
        beginObject();
        {
            Indent indent(indent_);
            bool exist = false;
            if(0<image.uri_.length()){
                exist = true;
                printObjectProperty("uri", image.uri_);
            }

            if(0<image.mimeType_.length()){
                exist = true;
                printObjectProperty("mimeType", image.mimeType_);
            }

            if(0<=image.bufferView_){
                exist = true;
                printObjectProperty("bufferView", image.bufferView_);
            }

            if(0<image.name_.length()){
                exist = true;
                printObjectProperty("name", image.name_);
            }

            if(image.extensions_.requiredOut()){
                exist = true;
                printObjectProperty("extensions", image.extensions_);
            }
            if(image.extras_.requiredOut()){
                exist = true;
                printObjectProperty("extras", image.extras_);
            }
            if(exist){
                replaceLastLine();
            }
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Material& material)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(0<material.name_.length()){
                printObjectProperty("name", material.name_);
            }

            if(material.extensions_.requiredOut()){
                printObjectProperty("extensions", material.extensions_);
            }
            if(material.extras_.requiredOut()){
                printObjectProperty("extras", material.extras_);
            }
            printObjectProperty("pbrMetallicRoughness", material.pbrMetallicRoughness_);
            if(0<=material.normalTexture_.index_){
                printObjectProperty("normalTexture", material.normalTexture_);
            }

            if(0<=material.occlusionTexture_.index_){
                printObjectProperty("occlusionTexture", material.occlusionTexture_);
            }

            if(0<=material.emissiveTexture_.index_){
                printObjectProperty("emissiveTexture", material.emissiveTexture_);
            }

            if(0.0f<material.emissiveFactor_[0]
                || 0.0f<material.emissiveFactor_[1]
                || 0.0f<material.emissiveFactor_[2])
            {
                printObjectProperty3("emissiveFactor", material.emissiveFactor_);
            }

            switch(material.alphaMode_){
            case GLTF_ALPHAMODE_OPAQUE:
                //printObjectProperty("alphaMode", GLTF_ALPHAMODE_NAME_OPAQUE);
                break;
            case GLTF_ALPHAMODE_MASK:
                printObjectProperty("alphaMode", GLTF_ALPHAMODE_NAME_MASK);
                break;
            case GLTF_ALPHAMODE_BLEND:
                printObjectProperty("alphaMode", GLTF_ALPHAMODE_NAME_BLEND);
                break;
            }
            if(!equal(material.alphaCutoff_, 0.5f)){
                printObjectProperty("alphaCutoff", material.alphaCutoff_);
            }
            if(material.doubleSided_){
                printObjectProperty("doubleSided", material.doubleSided_);
            }

            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const NormalTextureInfo& normalTextureInfo)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(normalTextureInfo.index_<0){
                return false;
            }
            printObjectProperty("index", normalTextureInfo.index_);

            if(0<=normalTextureInfo.texCoord_){
                printObjectProperty("texCoord", normalTextureInfo.texCoord_);
            }

            if(0.0f<=normalTextureInfo.scale_){
                printObjectProperty("scale", normalTextureInfo.scale_);
            }

            if(normalTextureInfo.extensions_.requiredOut()){
                printObjectProperty("extensions", normalTextureInfo.extensions_);
            }
            if(normalTextureInfo.extras_.requiredOut()){
                printObjectProperty("extras", normalTextureInfo.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const OcclusionTextureInfo& occlusionTextureInfo)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(occlusionTextureInfo.index_<0){
                return false;
            }
            printObjectProperty("index", occlusionTextureInfo.index_);

            if(0<=occlusionTextureInfo.texCoord_){
                printObjectProperty("texCoord", occlusionTextureInfo.texCoord_);
            }

            if(0.0f<=occlusionTextureInfo.strength_){
                printObjectProperty("strength", occlusionTextureInfo.strength_);
            }

            if(occlusionTextureInfo.extensions_.requiredOut()){
                printObjectProperty("extensions", occlusionTextureInfo.extensions_);
            }
            if(occlusionTextureInfo.extras_.requiredOut()){
                printObjectProperty("extras", occlusionTextureInfo.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const PbrMetallicRoughness& pbrMetallicRoughness)
    {
        beginObject();
        {
            Indent indent(indent_);

            bool exist = false;
            if(!equal(pbrMetallicRoughness.baseColorFactor_[0], 1.0f)
                || !equal(pbrMetallicRoughness.baseColorFactor_[1], 1.0f)
                || !equal(pbrMetallicRoughness.baseColorFactor_[2], 1.0f)
                || !equal(pbrMetallicRoughness.baseColorFactor_[3], 1.0f))
            {
                exist = true;
                printObjectProperty4("baseColorFactor", pbrMetallicRoughness.baseColorFactor_);
            }

            if(0<=pbrMetallicRoughness.baseColorTexture_.index_){
                exist = true;
                printObjectProperty("baseColorTexture", pbrMetallicRoughness.baseColorTexture_);
            }

            if(!equal(pbrMetallicRoughness.metallicFactor_, 1.0f)){
                exist = true;
                printObjectProperty("metallicFactor", pbrMetallicRoughness.metallicFactor_);
            }

            if(!equal(pbrMetallicRoughness.roughnessFactor_, 1.0f)){
                exist = true;
                printObjectProperty("roughnessFactor", pbrMetallicRoughness.roughnessFactor_);
            }

            if(0<=pbrMetallicRoughness.metallicRoughnessTexture_.index_){
                exist = true;
                printObjectProperty("metallicRoughnessTexture", pbrMetallicRoughness.metallicRoughnessTexture_);
            }

            if(pbrMetallicRoughness.extensions_.requiredOut()){
                exist = true;
                printObjectProperty("extensions", pbrMetallicRoughness.extensions_);
            }
            if(pbrMetallicRoughness.extras_.requiredOut()){
                exist = true;
                printObjectProperty("extras", pbrMetallicRoughness.extras_);
            }
            if(exist){
                replaceLastLine();
            }
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Mesh& mesh)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(mesh.primitives_.size()<=0){
                return false;
            }
            printArray("primitives", mesh.primitives_);

            if(0<mesh.weights_.size()){
                printArray("weights", mesh.weights_);
            }

            if(0<mesh.name_.length()){
                printObjectProperty("name", mesh.name_);
            }

            if(mesh.extensions_.requiredOut()){
                printObjectProperty("extensions", mesh.extensions_);
            }
            if(mesh.extras_.requiredOut()){
                printObjectProperty("extras", mesh.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Primitive& primitive)
    {
        beginObject();
        {
            Indent indent(indent_);
            if(primitive.attributes_.size()<=0){
                return false;
            }
            print("attributes", primitive.attributes_);
            printSeparatorLine();

            if(0<=primitive.indices_){
                printObjectProperty("indices", primitive.indices_);
            }

            if(0<=primitive.material_){
                printObjectProperty("material", primitive.material_);
            }

            if(GLTF_PRIMITIVE_POINTS<=primitive.mode_ && primitive.mode_<=GLTF_PRIMITIVE_TRIANGLE_FAN){
                printObjectProperty("mode", primitive.mode_);
            }

            if(0<primitive.targets_.size()){
                print("targets", primitive.targets_);
                printSeparatorLine();
            }

            if(primitive.extensions_.requiredOut()){
                printObjectProperty("extensions", primitive.extensions_);
            }
            if(primitive.extras_.requiredOut()){
                printObjectProperty("extras", primitive.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Char* key, const Array<Attribute>& attributes)
    {
        if(attributes.size()<=0){
            return false;
        }

        printIndent();
        print(key);
        printKeyValueSeparator();

        beginObject();
        {
            Indent indent(indent_);
            for(s32 i=0; i<attributes.size(); ++i){
                switch(attributes[i].semanticType_){
                case GLTF_ATTRIBUTE_POSITION:
                    printObjectProperty("POSITION", attributes[i].accessor_);
                    break;
                case GLTF_ATTRIBUTE_NORMAL:
                    printObjectProperty("NORMAL", attributes[i].accessor_);
                    break;
                case GLTF_ATTRIBUTE_TANGENT:
                    printObjectProperty("TANGENT", attributes[i].accessor_);
                    break;
                case GLTF_ATTRIBUTE_TEXCOORD:
                    switch(attributes[i].semanticIndex_){
                    case 0:
                        printObjectProperty("TEXCOORD_0", attributes[i].accessor_);
                        break;
                    case 1:
                        printObjectProperty("TEXCOORD_1", attributes[i].accessor_);
                        break;
                    case 2:
                        printObjectProperty("TEXCOORD_2", attributes[i].accessor_);
                        break;
                    default:
                        printObjectProperty("TEXCOORD_3", attributes[i].accessor_);
                        break;
                    }
                    break;
                case GLTF_ATTRIBUTE_COLOR:
                    switch(attributes[i].semanticIndex_){
                    case 0:
                        printObjectProperty("COLOR_0", attributes[i].accessor_);
                        break;
                    case 1:
                        printObjectProperty("COLOR_1", attributes[i].accessor_);
                        break;
                    case 2:
                        printObjectProperty("COLOR_2", attributes[i].accessor_);
                        break;
                    default:
                        printObjectProperty("COLOR_3", attributes[i].accessor_);
                        break;
                    }
                    break;
                case GLTF_ATTRIBUTE_JOINTS:
                    switch(attributes[i].semanticIndex_){
                    case 0:
                        printObjectProperty("JOINTS_0", attributes[i].accessor_);
                        break;
                    case 1:
                        printObjectProperty("JOINTS_1", attributes[i].accessor_);
                        break;
                    case 2:
                        printObjectProperty("JOINTS_2", attributes[i].accessor_);
                        break;
                    default:
                        printObjectProperty("JOINTS_3", attributes[i].accessor_);
                        break;
                    }
                    break;
                case GLTF_ATTRIBUTE_WEIGHTS:
                    switch(attributes[i].semanticIndex_){
                    case 0:
                        printObjectProperty("WEIGHTS_0", attributes[i].accessor_);
                        break;
                    case 1:
                        printObjectProperty("WEIGHTS_1", attributes[i].accessor_);
                        break;
                    case 2:
                        printObjectProperty("WEIGHTS_2", attributes[i].accessor_);
                        break;
                    default:
                        printObjectProperty("WEIGHTS_3", attributes[i].accessor_);
                        break;
                    }
                    break;
                }
            }
            replaceLastLine();

        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Char* key, const Array<MorphTarget>& targets)
    {
        if(targets.size()<=0){
            return false;
        }

        printIndent();
        print(key);
        printKeyValueSeparator();

        beginArray();
        {
            Indent indent(indent_);
            for(s32 i=0; i<targets.size(); ++i){
                beginObject();
                printObjectProperty("POSITION", targets[i].indices_[0]);
                printObjectProperty("NORMAL", targets[i].indices_[1]);

                printIndent();
                print("TANGENT");
                printKeyValueSeparator();
                print(targets[i].indices_[2]);
                printLine();
                endObject();
            }
            replaceLastLine();

        }
        endArray();
        return true;
    }

    bool glTFWriter::print(const Node& node)
    {
        beginObject();
        {
            Indent indent(indent_);

            if(0<=node.camera_){
                printObjectProperty("camera", node.camera_);
            }

            if(0<node.children_.size()){
                printArray("children", node.children_);
            }

            if(0<=node.skin_){
                printObjectProperty("skin", node.skin_);
            }

            if(node.flags_.check(Node::Flag_Matrix)){
                printObjectProperty16("matrix", node.matrix_);
            }

            if(0<=node.mesh_){
                printObjectProperty("mesh", node.mesh_);
            }

            if(node.flags_.check(Node::Flag_Rotation)){
                printObjectProperty4("rotation", node.rotation_);
            }
            if(node.flags_.check(Node::Flag_Scale)){
                printObjectProperty3("scale", node.scale_);
            }
            if(node.flags_.check(Node::Flag_Translation)){
                printObjectProperty3("translation", node.translation_);
            }
            if(0<node.weights_.size()){
                printArray("weights", node.weights_);
            }

            if(0<node.name_.length()){
                printObjectProperty("name", node.name_);
            }

            if(node.extensions_.requiredOut()){
                printObjectProperty("extensions", node.extensions_);
            }
            if(node.extras_.requiredOut()){
                printObjectProperty("extras", node.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Sampler& sampler)
    {
        beginObject();
        {
            Indent indent(indent_);

            switch(sampler.magFilter_){
            case GLTF_NEAREST:
            case GLTF_LINEAR:
            case GLTF_NEAREST_MIPMAP_NEAREST:
            case GLTF_LINEAR_MIPMAP_NEAREST:
            case GLTF_NEAREST_MIPMAP_LINEAR:
            case GLTF_LINEAR_MIPMAP_LINEAR:
                printObjectProperty("magFilter", sampler.magFilter_);
                break;
            }
            switch(sampler.minFilter_){
            case GLTF_NEAREST:
            case GLTF_LINEAR:
            case GLTF_NEAREST_MIPMAP_NEAREST:
            case GLTF_LINEAR_MIPMAP_NEAREST:
            case GLTF_NEAREST_MIPMAP_LINEAR:
            case GLTF_LINEAR_MIPMAP_LINEAR:
                printObjectProperty("minFilter", sampler.minFilter_);
                break;
            }

            switch(sampler.wrapS_){
            case GLTF_REPEAT:
            case GLTF_CLAMP_TO_EDGE:
            case GLTF_MIRRORED_REPEAT:
                printObjectProperty("wrapS", sampler.wrapS_);
                break;
            }
            switch(sampler.wrapT_){
            case GLTF_REPEAT:
            case GLTF_CLAMP_TO_EDGE:
            case GLTF_MIRRORED_REPEAT:
                printObjectProperty("wrapT", sampler.wrapT_);
                break;
            }

            if(0<sampler.name_.length()){
                printObjectProperty("name", sampler.name_);
            }

            if(sampler.extensions_.requiredOut()){
                printObjectProperty("extensions", sampler.extensions_);
            }
            if(sampler.extras_.requiredOut()){
                printObjectProperty("extras", sampler.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Scene& scene)
    {
        beginObject();
        {
            Indent indent(indent_);

            if(0<scene.nodes_.size()){
                printArray("nodes", scene.nodes_);
            }

            if(0<scene.name_.length()){
                printObjectProperty("name", scene.name_);
            }

            if(scene.extensions_.requiredOut()){
                printObjectProperty("extensions", scene.extensions_);
            }
            if(scene.extras_.requiredOut()){
                printObjectProperty("extras", scene.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Skin& skin)
    {
        beginObject();
        {
            Indent indent(indent_);

            if(0<=skin.inverseBindMatrices_){
                printObjectProperty("inverseBindMatrices", skin.inverseBindMatrices_);
            }
            if(0<=skin.skeleton_){
                printObjectProperty("skeleton", skin.skeleton_);
            }

            if(0<skin.joints_.size()){
                printArray("joints", skin.joints_);
            }

            if(0<skin.name_.length()){
                printObjectProperty("name", skin.name_);
            }

            if(skin.extensions_.requiredOut()){
                printObjectProperty("extensions", skin.extensions_);
            }
            if(skin.extras_.requiredOut()){
                printObjectProperty("extras", skin.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const Texture& texture)
    {
        beginObject();
        {
            Indent indent(indent_);

            if(0<=texture.sampler_){
                printObjectProperty("sampler", texture.sampler_);
            }
            if(0<=texture.source_){
                printObjectProperty("source", texture.source_);
            }

            if(0<texture.name_.length()){
                printObjectProperty("name", texture.name_);
            }

            if(texture.extensions_.requiredOut()){
                printObjectProperty("extensions", texture.extensions_);
            }
            if(texture.extras_.requiredOut()){
                printObjectProperty("extras", texture.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }

    bool glTFWriter::print(const TextureInfo& textureInfo)
    {
        beginObject();
        {
            Indent indent(indent_);

            if(0<=textureInfo.index_){
                printObjectProperty("index", textureInfo.index_);
            }
            if(0<=textureInfo.texCoord_){
                printObjectProperty("texCoord", textureInfo.texCoord_);
            }

            if(textureInfo.extensions_.requiredOut()){
                printObjectProperty("extensions", textureInfo.extensions_);
            }
            if(textureInfo.extras_.requiredOut()){
                printObjectProperty("extras", textureInfo.extras_);
            }
            replaceLastLine();
        }
        endObject();
        return true;
    }
}
#endif //GLTF_IMPLEMENTATION
