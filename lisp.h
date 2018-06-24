#pragma once

#include <map>
#include <list>
#include <string>
#include <vector>
#include <functional>

struct token {
    int type;
    std::string buffer;
};

//struct Type {
//    std::string name;
//    std::vector<Type> base;
//
//    std::string mangle() {
//        std::string type;
//        type.append(std::to_string(name.size()));
//        type.append(name);
//        return type;
//    }
//
//    bool is_base_of(Type other) {
//        if (name == other.name) {
//            return true;
//        }
//
//        for (auto& type : base) {
//            if (type.is_base_of(type)) {
//                return true;
//            }
//        }
//
//        return false;
//    }
//};
//
//struct Method {
//    std::string name;
//
//    Type result;
//    std::vector<Type> args;
//
//    std::string mangle() {
//        std::string type;
//        type.append(std::to_string(name.size()));
//        for (auto& arg : args) {
//            type.append(arg.mangle());
//        }
//        return type;
//    }
//};

struct lisp {
    struct Env;
    struct Cell;

    typedef std::vector<Cell> List;
    typedef lisp::Cell(*Func)(Env*, const List&);
    //typedef lisp::Cell(*Macro)(Env*, Cell);

    struct Symbol : std::string {
        Symbol(const std::string& str) : std::string(str) {}
    };

    struct Cell {
    public:
        enum class Type { Symbol, Number, List, Procedure, Lambda, Pointer, String } type;

        void* ptr;
        int number{};
        std::string text;
        std::string symbol;
        std::vector<Cell> list;
        Cell(*proc)(Env*, const List&){};
        Env* env = nullptr;
        int lambda {};

        Cell();
        Cell(void* ptr);
        Cell(int number);
        Cell(Symbol symbol);
        Cell(std::string text);
        Cell(List list);
        Cell(Func proc);

        std::string get_typename(Env *env) const;

        std::string to_string() const;

        bool _quote_ = false;
        bool _super_ = false;
        bool _inline_ = false;
        bool _variadic_ = false;
    };

    struct Env {
    public:
        std::map<std::string, Cell> table{};
        Env *super = nullptr;
        std::string context;

        Cell& Find(const std::string& name);

        void dump();
    };

    static void dump(Env* env, const std::string &source);
    static void dump(Env* env, Cell cell);

    static Cell eval(Env *env, const std::string &source);
    static Cell eval(Env* env, Cell cell);

    static std::string mangle(Env *env, const std::string &decl);

    static std::string demangle(const std::string &name);

//private:
    static Cell parse(const std::string &source);
    static Cell parse(std::vector<struct token>::iterator &tokens);

    static Cell apply(Env *env, Cell cell);

    static void compile(const std::string &source);
};