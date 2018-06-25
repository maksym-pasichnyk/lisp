#include "lisp-jit.h"

#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdint>
#include <cstdlib>
#include <cstdio>

static inline void* create_block(size_t size) {
    return mmap(nullptr, size, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

static inline int make_executable(void* block, size_t size) {
    return mprotect(block, size, PROT_READ | PROT_EXEC);
}

static inline void gen_imm(std::vector<uint8_t>& code, int imm) {
    auto val = reinterpret_cast<char*>(&imm);

    for (size_t idx = 0; idx < sizeof(int); idx++) {
        code.push_back(uint8_t(val[idx]));
    }
}

static inline void gen_ptr(std::vector<uint8_t> &code, void* ptr) {
    auto val = reinterpret_cast<char*>(&ptr);

    for (size_t idx = 0; idx < sizeof(void*); idx++) {
        code.push_back(uint8_t(val[idx]));
    }
}

static inline void gen_mov_rax_ptr(std::vector<uint8_t> &code, void *ptr) {
    code.push_back(0x48);
    code.push_back(0xb8);

    gen_ptr(code, ptr);
}

static inline void gen_mov_rcx_ptr(std::vector<uint8_t>& code, void* ptr) {
    code.push_back(0x48);
    code.push_back(0xb9);
    gen_ptr(code, ptr);
}

static inline void gen_mov_rdx_ptr(std::vector<uint8_t>& code, void* ptr) {
    code.push_back(0x48);
    code.push_back(0xba);
    gen_ptr(code, ptr);
}

static inline void gen_mov_rsi_ptr(std::vector<uint8_t>& code, void* ptr) {
    code.push_back(0x48);
    code.push_back(0xbe);
    gen_ptr(code, ptr);
}

static inline void gen_mov_rdi_ptr(std::vector<uint8_t>& code, void* ptr) {
    code.push_back(0x48);
    code.push_back(0xbf);
    gen_ptr(code, ptr);
}

static inline void gen_mov_r8_ptr(std::vector<uint8_t>& code, void* ptr) {
    code.push_back(0x49);
    code.push_back(0xb8);
    gen_ptr(code, ptr);
}

static inline void gen_mov_r9_ptr(std::vector<uint8_t>& code, void* ptr) {
    code.push_back(0x49);
    code.push_back(0xb9);
    gen_ptr(code, ptr);
}

static inline void gen_push_ptr(std::vector<uint8_t>& code, void* ptr) {
    gen_mov_rax_ptr(code, ptr);
    code.push_back(0x50);
}

static inline void gen_mov_eax_imm(std::vector<uint8_t>& code, int imm) {
    code.push_back(0xb8);
    gen_imm(code, imm);
}

static inline void gen_mov_ecx_imm(std::vector<uint8_t>& code, int imm) {
    code.push_back(0xb9);
    gen_imm(code, imm);
}

static inline void gen_mov_edx_imm(std::vector<uint8_t>& code, int imm) {
    code.push_back(0xba);
    gen_imm(code, imm);
}

static inline void gen_mov_esi_imm(std::vector<uint8_t>& code, int imm) {
    code.push_back(0xbe);
    gen_imm(code, imm);
}

static inline void gen_mov_edi_imm(std::vector<uint8_t>& code, int imm) {
    code.push_back(0xbf);
    gen_imm(code, imm);
}

static inline void gen_mov_r8d_imm(std::vector<uint8_t>& code, int imm) {
    code.push_back(0x41);
    code.push_back(0xb8);
    gen_imm(code, imm);
}

static inline void gen_mov_r9d_imm(std::vector<uint8_t>& code, int imm) {
    code.push_back(0x41);
    code.push_back(0xb9);
    gen_imm(code, imm);
}

static inline void gen_push_imm(std::vector<uint8_t>& code, int imm) {
    code.push_back(0x68);
    gen_imm(code, imm);
}

BlockPtr new_func(void *ptr, const std::vector<Argument> &args) {
    std::vector<uint8_t> prepare_args;

    uint8_t push_size = 0x00;

    if (!args.empty()) {
        typedef void(*MovReg32Imm)(std::vector<uint8_t>&, int);
        typedef void(*MovReg64Ptr)(std::vector<uint8_t>&, void*);

        static MovReg32Imm mov_reg32_imm[] = {
            gen_mov_edi_imm,
            gen_mov_esi_imm,
            gen_mov_edx_imm,
            gen_mov_ecx_imm,
            gen_mov_r8d_imm,
            gen_mov_r9d_imm,
        };

        static MovReg64Ptr mov_reg64_ptr[] = {
            gen_mov_rdi_ptr,
            gen_mov_rsi_ptr,
            gen_mov_rdx_ptr,
            gen_mov_rcx_ptr,
            gen_mov_r8_ptr,
            gen_mov_r9_ptr,
        };

        auto count1 = args.size();
        auto count2 = std::min(count1, size_t(6));

        for (size_t i = 0; i < count2; i++) {
            switch (args[i].type) {
                case Argument::Int:
                    mov_reg32_imm[i](prepare_args, args[i].i);
                    break;
                case Argument::Ptr:
                    mov_reg64_ptr[i](prepare_args, args[i].p);
                    break;
                case Argument::Str:
                    mov_reg64_ptr[i](prepare_args, (void*)args[i].s);
                    break;
                default:
                    fprintf(stderr, "codegen: unsupported type '%i'", args[i].type);
                    exit(1);
            }
        }

        for (size_t i = 6; i < count1; i++) {
            switch (args[i].type) {
                case Argument::Int:
                    gen_push_imm(prepare_args, args[i].i);
                    push_size += 8;
                    break;
                case Argument::Ptr:
                    gen_push_ptr(prepare_args, args[i].p);
                    push_size += 8;
                    break;
                case Argument::Str:
                    gen_push_ptr(prepare_args, (void*)args[i].s);
                    push_size += 8;
                    break;
                default:
                    fprintf(stderr, "codegen: unsupported type '%i'", args[i].type);
                    exit(1);
            }
        }
    }

    auto sub_sp = uint8_t(push_size % 16 + 0x8);
    auto add_sp = uint8_t(sub_sp + push_size);

    std::vector<uint8_t> code;
    code.insert(code.end(), { 0x48, 0x83, 0xec, sub_sp });
    code.insert(code.end(), prepare_args.begin(), prepare_args.end());
    gen_mov_rax_ptr(code, ptr);
    code.insert(code.end(), { 0xff, 0xd0 });
    code.insert(code.end(), { 0x48, 0x83, 0xc4, add_sp });
    code.push_back(0xc3);

    auto block_size = size_t(sysconf(_SC_PAGESIZE));
    auto block = create_block(block_size);

    memcpy(block, &code[0], code.size());

    make_executable(block, block_size);

    return { block, block_size };
}

void del_func(BlockPtr block) {
    munmap(block.addr, block.len);
}
