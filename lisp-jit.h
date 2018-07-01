#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <cxxabi.h>
#include "lisp-type.h"

struct BlockPtr {
    void* addr = nullptr;
    size_t len = 0;
};

struct Argument {
    Type type;

    union {
        int8_t i8;
        uint8_t u8;

        int16_t i16;
        uint16_t u16;

        int32_t i32;
        uint32_t u32;

        int64_t i64;
        uint64_t u64;

        void* p;
    };

    std::string s;

    explicit Argument(Type type) : type(type) {}

    static inline Argument NewInt8(int8_t i8) {
        static Argument val(get_type<int8_t>());
        val.i8 = i8;
        return val;
    }

    static inline Argument NewInt16(int8_t i16) {
        static Argument val(get_type<int16_t>());
        val.i16 = i16;
        return val;
    }

    static inline Argument NewInt32(int32_t i32) {
        static Argument val(get_type<int32_t>());
        val.i32 = i32;
        return val;
    }

    static inline Argument NewInt64(int64_t i64) {
        static Argument val(get_type<int64_t>());
        val.i64 = i64;
        return val;
    }

    static inline Argument NewPtr(void *p) {
        static Argument val(get_type<void*>());
        val.p = p;
        return val;
    }

    static inline Argument NewStr(const std::string& s) {
        static Argument val(get_type<std::string>());
        val.s = s;
        return val;
    }
};

extern BlockPtr new_func(void *ptr, Type return_type, const std::vector<Type>& args_types, const std::vector<Argument>& args);
extern void del_block(BlockPtr block);