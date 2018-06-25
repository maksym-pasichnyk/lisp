#pragma once

#include <vector>
#include <cstdint>

struct BlockPtr {
    void* addr = nullptr;
    size_t len = 0;
};

struct Value {
    enum : uint8_t { Int, Ptr, Struct };

    uint8_t type;
    int number = 0;

    explicit Value(uint8_t type) : type(type) {}

    static Value make_int(int i) {
        Value val(Int);
        val.number = i;
        return val;
    }
};

extern BlockPtr new_func(void *ptr, const std::vector<Value> &args);
extern void free_block(BlockPtr block);