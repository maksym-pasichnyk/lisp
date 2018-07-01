#pragma once

#include <map>
#include <list>
#include <string>
#include <vector>
#include <functional>
#include "lisp-jit.h"

struct lisp {
private:
    struct FuncPtr;

public:
    struct Env;
    struct Cell;

    using List = std::vector<Cell>;
    using Proc = lisp::Cell(lisp::Env*, const lisp::List&);
    struct Symbol : std::string {
        explicit Symbol(const std::string& str) : std::string(str) {}
    };

    struct CFunc {
        FuncPtr* func = nullptr;

        CFunc() = default;

        template <typename T, typename ...Args>
        explicit CFunc(T(*ptr)(Args...)) : func(new Func<T(Args...)>(ptr)) {}

        lisp::Cell invoke(const std::vector<Argument>& args);
    };

    struct Lambda {};

    struct Cell {
    public:
        Type type;

        union {
            bool b;

            int8_t i8;
            uint8_t u8;

            int16_t i16;
            uint16_t u16;

            int32_t i32;
            uint32_t u32;

            int64_t i64;
            uint64_t u64{};

            float f;
            double d;

            void* ptr;
            Proc* proc;
        };

        std::string s;

        std::vector<Cell> list;
        CFunc func;
        Env* env = nullptr;

        Cell() : type(get_type<void>()) {}

        template <typename T> Cell(T val) = delete;

        Cell(void* val) : type(get_type<void*>()), ptr(val) {}
        Cell(bool val) : type(get_type<bool>()), b(val) {}
        Cell(char val) : type(get_type<char>()), i8(val) {}
        Cell(int val) : type(get_type<int>()), i32(val)  {}
        Cell(float val) : type(get_type<float>()), f(val) {}
        Cell(double val) : type(get_type<double>()), d(val) {}
        Cell(Symbol val) : type(get_type<Symbol>()), s(std::move(val)) {}
        Cell(std::string val) : type(get_type<std::string>()), s(std::move(val)) {}
        Cell(List val) : type(get_type<List>()), list(std::move(val)) {}
        Cell(Proc val) : type(get_type<Proc>()), proc(val) {}
        Cell(CFunc val) : type(get_type<CFunc>()), func(val) {}

        template <typename T>
        Cell(T* val) : type(get_type<void*>()), ptr((void*)val) {}

        std::string to_string() const;

        bool _quote_ = false;
        bool _super_ = false;
        bool _inline_ = false;
        bool _variadic_ = false;
    };

    struct Env {
    public:
        std::map<std::string, Cell> table;
        Env* super = nullptr;

        Cell& Find(const std::string& name);

        void dump();
    };

    static Cell eval(Env *env, const char* source);
    static Cell eval(Env* env, Cell cell);

    static Cell compile(const char* source);
private:
    struct FuncPtr {
        virtual lisp::Cell invoke(const std::vector<Argument>&) = 0;
    };

    template <typename T>
    struct Func;

    template <typename T, typename ...Args>
    struct Func <T(Args...)> final : FuncPtr {
    private:
        void* ptr;
        ::Type return_type;
        std::vector<::Type> args_types;

    public:
        explicit inline Func(T(*ptr)(Args...)) : ptr((void*) ptr), return_type(get_type<T>()), args_types({ get_type<Args>()... }) {}

        lisp::Cell invoke(const std::vector<Argument>& args) final {
            BlockPtr block = new_func(ptr, return_type, args_types, args);

            if constexpr (std::is_void<T>::value) {
                ((void(*)()) block.addr)();
                del_block(block);
                return 0;
            } else {
                auto res = ((T(*)()) block.addr)();
                del_block(block);
                return lisp::Cell(res);
            }
        }
    };

    static Cell parse(const std::string &source);
    static Cell parse(std::vector<struct token>::iterator &tokens);

    static Cell apply(Env *env, Cell cell);

    static Cell compile(Env *env, Cell cell);
};