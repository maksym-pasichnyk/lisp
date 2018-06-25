#pragma once

#include <vector>
#include <cstdint>

struct BlockPtr {
    void* addr = nullptr;
    size_t len = 0;
};

struct Argument {
    enum : uint8_t { Int, Ptr, Str };

    uint8_t type;
    int i = 0;
    void* p = nullptr;
    const char* s = nullptr;

    explicit Argument(uint8_t type) : type(type) {}

    static inline Argument make_int(int i) {
        Argument val(Int);
        val.i = i;
        return val;
    }

    static inline Argument make_ptr(void* p) {
        Argument val(Ptr);
        val.p = p;
        return val;
    }

    static inline Argument make_str(const char *s) {
        Argument val(Str);
        val.s = s;
        return val;
    }
};

extern BlockPtr new_func(void *ptr, const std::vector<Argument> &args);
extern void del_func(BlockPtr block);