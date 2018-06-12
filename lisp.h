#pragma once

#include <map>
#include <list>
#include <string>
#include <vector>
#include <functional>

struct lisp {
    struct Env;
    struct Cell;

    typedef const std::vector<Cell>& Args;
    typedef lisp::Cell(*Func)(Env*, Args);

    struct Symbol : std::string {};

    struct Cell {
    public:
        enum class Type { Symbol, Number, List, Procedure, Lambda, Pointer, String } type;

        void* ptr;
        int number{};
        std::string text;
        std::string symbol;
        std::vector<Cell> list;
        Cell(*proc)(Env*, Args){};
        Env* env = nullptr;
        int lambda {};

        Cell();
        Cell(void* ptr);
        Cell(int number);
        Cell(Symbol symbol);
        Cell(std::string text);
        Cell(std::vector<Cell> list);
        Cell(Cell(*proc)(Env*, Args));

        std::string to_string() const;
    };

    struct Env {
    public:
        std::map<std::string, Cell> table{};
        Env *super = nullptr;

        Cell& Find(const std::string& name);

        void dump();

        int unique = 0;
    };

    static Cell eval(Env *env, const std::string &source);
    static Cell eval(Env* env, Cell cell);

private:
    static Cell parse(const std::string &source);
    static Cell parse(std::vector<struct token>::iterator &tokens);
};
