#pragma once

#include <map>
#include <vector>
#include <string>
#include <functional>

struct lisp {
    struct Cell;

    typedef const std::vector<Cell>& Args;

    struct Env;
    struct Cell {
    public:
        typedef std::vector<Cell>::iterator iterator;
        typedef std::vector<Cell>::const_iterator const_iterator;

        enum Type { Symbol, Number, List, Proc, Lambda } type;
        int number{};
        std::string symbol;
        std::vector<Cell> list;
        Cell(*proc)(Env*, Args){};
        std::function<Cell(Env*, Cell)> lambda;

        Cell(Type type = Symbol);
        Cell(int number);
        Cell(std::string symbol);
        Cell(std::vector<Cell> list);
        Cell(Cell(*proc)(Env*, Args));
        Cell(std::function<Cell(Env*, Cell)> lambda);

        inline Cell& operator[](int index);
        inline const Cell& operator[](int index) const;
        inline iterator begin();
        inline const_iterator begin() const;
        inline iterator end();
        inline const_iterator end() const;

        inline Cell operator()(Env* env, Args args);

        inline std::string to_string() const;
    };

    struct Env {
    public:
        std::map<std::string, Cell> table;
        Env *super;

        Env(Args args, Args values, Env* super);

        Cell& Find(const std::string& name);
    };

    static Cell eval(Env* env, const std::string& source);

private:
    static Cell parse(const std::string &source);
    static Cell eval(Env* env, Cell cell);
    static Cell parse(std::vector<struct token>::iterator &tokens);
};
