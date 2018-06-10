#pragma once

#include <map>
#include <list>
#include <string>
#include <vector>
#include <functional>

struct lisp {
    struct Cell;

    typedef const std::vector<Cell>& Args;

    struct Env;
    struct Cell {
    public:
        enum Type { Symbol, Number, List, Proc, Lambda, Pointer } type;

        void* ptr;
        int number{};
        std::string symbol;
        std::vector<Cell> list;
        Cell(*proc)(Env*, Args){};

        Cell(Type type = Symbol);
        Cell(void* ptr);
        Cell(int number);
        Cell(std::string symbol);
        Cell(std::vector<Cell> list);
        Cell(Cell(*proc)(Env*, Args));

        std::string to_string() const;
    };

    struct Env {
    public:
        std::map<std::string, Cell> table;
        Env *super;

        Env(Args args, Args values, Env* super);

        Cell& Find(const std::string& name);
    };

    static Cell eval(Env* env, const std::string& source);
    static Cell eval(Env* env, Cell cell);

private:
    static Cell parse(const std::string &source);
    static Cell parse(std::vector<struct token>::iterator &tokens);
};
