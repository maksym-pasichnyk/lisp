#pragma once

#include <map>
#include <list>
#include <string>
#include <vector>
#include <functional>
#include "lisp-jit.h"

struct lisp {
    enum class Type { Symbol, Number, List, Proc, Lambda, Ptr, String, Func };

    struct Env;
    struct Cell;

    struct FuncPtr {
        virtual lisp::Cell invoke(const std::vector<Argument>&) = 0;
    };

    template <typename T>
    struct Func;

    template <typename T, typename ...Args>
    struct Func <T(Args...)> : FuncPtr {
        void* ptr;

        Func(void* ptr) : ptr(ptr) {}
        Func(T(*ptr)(Args...)) : ptr((void*) ptr) {}

        virtual lisp::Cell invoke(const std::vector<Argument>& args) override {
            BlockPtr block = new_func(ptr, args);

            auto res = ((T(*)()) block.addr)();
            del_func(block);
            return res;
        }
    };

    template <typename ...Args>
    struct Func <void(Args...)> : FuncPtr {
        void* ptr;

        Func(void* ptr) : ptr(ptr) {}
        Func(void(*ptr)(Args...)) : ptr((void*) ptr) {}

        virtual lisp::Cell invoke(const std::vector<Argument>& args) override {
            BlockPtr block = new_func(ptr, args);

            ((void(*)()) block.addr)();
            del_func(block);
            return (void*) nullptr;
        }
    };

    typedef std::vector<Cell> List;
    typedef lisp::Cell(*Proc)(Env*, const List&);

    struct Symbol : std::string {
        Symbol(const std::string& str) : std::string(str) {}
    };

    struct CFunc {
        FuncPtr* func;

        CFunc() {}

        template <typename T, typename ...Args>
        CFunc(void* ptr) : func(new Func<T(Args...)>(ptr)) {}

        template <typename T, typename ...Args>
        CFunc(T(*ptr)(Args...)) : func(new Func<T(Args...)>(ptr)) {}

        lisp::Cell invoke(const std::vector<Argument>& args);
    };

    struct Cell {
    public:
        Type type;

        void* ptr = nullptr;
        int number = 0;
        std::string text;
        std::string symbol;
        std::vector<Cell> list;
        Proc proc = nullptr;
        CFunc func;
        Env* env = nullptr;

        Cell();
        Cell(void* ptr);
        Cell(int number);
        Cell(Symbol symbol);
        Cell(std::string text);
        Cell(List list);
        Cell(Proc proc);
        Cell(CFunc func);

        std::string get_typename(Env *env) const;

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

    static Cell eval(Env *env, const std::string &source);
    static Cell eval(Env* env, Cell cell);

private:
    static Cell parse(const std::string &source);
    static Cell parse(std::vector<struct token>::iterator &tokens);

    static Cell apply(Env *env, Cell cell);
};