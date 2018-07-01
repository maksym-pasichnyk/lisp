#include "lisp-jit.h"

#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <dlfcn.h>

static inline void* create_block(size_t size) {
    return mmap(nullptr, size, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

static inline int make_executable(void* block, size_t size) {
    return mprotect(block, size, PROT_READ | PROT_EXEC);
}

void** find_vtable(char *symbol) {
    return (void **)((size_t)dlsym(RTLD_DEFAULT, symbol) + 8);
}

int get_vtable_index(void **vtable, char *symbol) {
    int index = 0;
    while (*vtable) {
        Dl_info info;
        dladdr(*vtable++, &info);
        if (strstr(info.dli_sname, symbol))
            return index;
        index++;
    }
    return -1;
}

static inline void imm32(std::vector<uint8_t>& code, uint32_t imm) {
    auto val = reinterpret_cast<uint8_t*>(&imm);

    for (size_t idx = 0; idx < sizeof(uint32_t); idx++) {
        code.push_back(val[idx]);
    }
}

static inline void imm64(std::vector<uint8_t>& code, uint64_t imm) {
    auto val = reinterpret_cast<uint8_t*>(&imm);

    for (size_t idx = 0; idx < sizeof(uint64_t); idx++) {
        code.push_back(val[idx]);
    }
}

static inline void mov_rax_imm(std::vector<uint8_t>& code, uint64_t imm) {
    code.push_back(0x48);
    code.push_back(0xb0 + 0x08);
    imm64(code, imm);
}

static inline void mov_rcx_imm(std::vector<uint8_t>& code, uint64_t imm) {
    code.push_back(0x48);
    code.push_back(0xb0 + 0x09);
    imm64(code, imm);
}

static inline void mov_rdx_imm(std::vector<uint8_t>& code, uint64_t imm) {
    code.push_back(0x48);
    code.push_back(0xb0 + 0x0a);
    imm64(code, imm);
}

static inline void mov_rsi_imm(std::vector<uint8_t>& code, uint64_t imm) {
    code.push_back(0x48);
    code.push_back(0xb0 + 0x0e);
    imm64(code, imm);
}

static inline void mov_rdi_imm(std::vector<uint8_t>& code, uint64_t imm) {
    code.push_back(0x48);
    code.push_back(0xb0 + 0x0f);
    imm64(code, imm);
}

static inline void mov_r8_imm(std::vector<uint8_t>& code, uint64_t imm) {
    code.push_back(0x49);
    code.push_back(0xb0 + 0x08);
    imm64(code, imm);
}

static inline void mov_r9_imm(std::vector<uint8_t>& code, uint64_t imm) {
    code.push_back(0x49);
    code.push_back(0xb0 + 0x09);
    imm64(code, imm);
}

static inline void mov_eax_imm(std::vector<uint8_t>& code, uint32_t imm) {
    code.push_back(0xb0 + 0x08);
    imm32(code, imm);
}

static inline void mov_ecx_imm(std::vector<uint8_t>& code, uint32_t imm) {
    code.push_back(0xb0 + 0x09);
    imm32(code, imm);
}

static inline void mov_edx_imm(std::vector<uint8_t>& code, uint32_t imm) {
    code.push_back(0xb0 + 0x0a);
    imm32(code, imm);
}

static inline void mov_esi_imm(std::vector<uint8_t>& code, uint32_t imm) {
    code.push_back(0xb0 + 0x0e);
    imm32(code, imm);
}

static inline void mov_edi_imm(std::vector<uint8_t>& code, uint32_t imm) {
    code.push_back(0xb0 + 0x0f);
    imm32(code, imm);
}

static inline void mov_r8d_imm(std::vector<uint8_t>& code, uint32_t imm) {
    code.push_back(0x41);
    code.push_back(0xb0 + 0x08);
    imm32(code, imm);
}

static inline void mov_r9d_imm(std::vector<uint8_t>& code, uint32_t imm) {
    code.push_back(0x41);
    code.push_back(0xb0 + 0x09);
    imm32(code, imm);
}

static inline void push_imm32(std::vector<uint8_t>& code, uint32_t imm) {
    code.push_back(0x68);
    imm32(code, imm);
}

BlockPtr new_func(void *ptr, Type return_type, const std::vector<Type>& args_types, const std::vector<Argument>& args) {
    std::vector<uint8_t> prepare_args;

    uint8_t push_size = 0x00;

    if (!args.empty()) {
        typedef void(*MovReg32Imm)(std::vector<uint8_t>&, uint32_t);
        typedef void(*MovReg64Imm)(std::vector<uint8_t>&, uint64_t);

        static MovReg32Imm mov_reg32_imm[] = {
            mov_edi_imm,
            mov_esi_imm,
            mov_edx_imm,
            mov_ecx_imm,
            mov_r8d_imm,
            mov_r9d_imm,
        };

        static MovReg64Imm mov_reg64_imm[] = {
            mov_rdi_imm,
            mov_rsi_imm,
            mov_rdx_imm,
            mov_rcx_imm,
            mov_r8_imm,
            mov_r9_imm,
        };

        auto count1 = args.size();
        auto count2 = std::min(count1, size_t(6));

        for (size_t i = 0; i < count2; i++) {
            auto const& type = args[i].type;
            auto hash_code = type->hash_code();

            if (hash_code == typeid(int32_t).hash_code()) {
                mov_reg32_imm[i](prepare_args, args[i].u32);
            } else if (hash_code == typeid(int64_t).hash_code()) {
                mov_reg64_imm[i](prepare_args, args[i].u64);
            } else if (type->is_pointer()) {
                mov_reg64_imm[i](prepare_args, args[i].u64);
            } else if (hash_code == typeid(std::string).hash_code()) {
                mov_reg64_imm[i](prepare_args, uint64_t(args[i].s.c_str()));
            } else {
                fprintf(stderr, "codegen: unsupported type '%lu'", type->pretty_name());
                exit(1);
            }
        }

        for (size_t i = 6; i < count1; i++) {
            auto const& type = args[i].type;
            auto hash_code = type->hash_code();

            if (hash_code == typeid(int32_t).hash_code()) {
                push_imm32(prepare_args, args[i].u32);
                push_size += 8;
            } else if (hash_code == typeid(int64_t).hash_code()) {
                mov_rax_imm(prepare_args, args[i].u64);
                prepare_args.push_back(0x50);
                push_size += 8;
            } else if (type->is_pointer()) {
                mov_rax_imm(prepare_args, args[i].u64);
                prepare_args.push_back(0x50);
                push_size += 8;
            } else if (hash_code == typeid(std::string).hash_code()) {
                mov_rax_imm(prepare_args, uint64_t(args[i].s.c_str()));
                prepare_args.push_back(0x50);
                push_size += 8;
            } else {
                fprintf(stderr, "codegen: unsupported type '%lu'", type->pretty_name());
                exit(1);
            }
        }
    }

    auto sub_sp = uint8_t(push_size % 16 + 0x8 + 0x8);
    auto add_sp = uint8_t(sub_sp + push_size);

    std::vector<uint8_t> code;
    code.push_back(0x55);
//    code.push_back(0x55);
    code.insert(code.end(), { 0x48, 0x83, 0xec, sub_sp });
    code.insert(code.end(), prepare_args.begin(), prepare_args.end());
    mov_rax_imm(code, uint64_t(ptr));
    code.insert(code.end(), { 0xff, 0xd0 });
//    code.push_back(0xc9);
    code.insert(code.end(), { 0x48, 0x83, 0xc4, add_sp });
    code.push_back(0x5d);
    code.push_back(0xc3);

    auto block_size = size_t(sysconf(_SC_PAGESIZE));
    auto block = create_block(block_size);

    memcpy(block,& code[0], code.size());

    make_executable(block, block_size);

    return { block, block_size };
}

void del_block(BlockPtr block) {
    munmap(block.addr, block.len);
}