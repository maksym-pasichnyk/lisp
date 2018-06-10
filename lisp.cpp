#include "lisp.h"

static const unsigned char attr[256] = {
    0,
    1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
    2, 2, 2, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 2, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 5, 5, 5,
    5, 5, 5, 5, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 5, 5, 5, 5, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1
};

static const unsigned char accept[6] = {
    0,
    0, 0, 3, 4, 5
};

static const char next[6][6] = {
     0,  0,  0,  0,  0,  0,
     0,  1,  2,  3,  4,  5,
    -2, -2,  2, -2, -2, -2,
    -3, -3, -3,  3, -3, -3,
    -4, -4, -4, -4,  4, -4,
    -5, -5, -5, -5, -5, -5
};

struct token {
    int type;
    std::string buffer;
};

static std::vector<token> lex(const char* input) {
    std::vector<token> out;

    while (*input) {
        int state = 1;

        const char* buf = input;
        while ((state = next[state][attr[*input]]) > 0) {
            input++;
        }
        state = -state;

        switch (accept[state]) {
            case 3: out.push_back({256, std::string(buf, input - buf)}); break;
            case 4: out.push_back({257, std::string(buf, input - buf)}); break;
            case 5: out.push_back({buf[0], std::string(buf, input - buf)}); break;
            default: break;
        }
    }

    return out;
}

lisp::Cell::Cell(Cell::Type type) : type(type) {}
lisp::Cell::Cell(int number) : type(Number), number(number) {}
lisp::Cell::Cell(std::string symbol) : type(Symbol), symbol(std::move(symbol)) {}
lisp::Cell::Cell(std::vector<Cell> list) : type(List), list(std::move(list)) {}
lisp::Cell::Cell(Cell(*proc)(Env* , Args)) : type(Proc), proc(proc) {}

lisp::Cell lisp::Cell::operator()(lisp::Env* env, lisp::Args args) {
    if (type == lisp::Cell::Proc) {
        return proc(env, args);
    }
    if (type == lisp::Cell::Lambda) {
        auto context = new Env(list[1].list, args, env);
        auto result = eval(context, list[2]);
        delete context;
        return result;
    }
    return 0;
}

std::string lisp::Cell::to_string() const {
    if (type == Type::Symbol) return symbol;
    if (type == Type::Number) return std::to_string(number);
    if (type == Type::List) {
        std::string str;
        if (!list.empty()) {
            for (const auto &e : list) str.append(e.to_string()).push_back(' ');
            str.pop_back();
        }
        return "(" + str + ")";
    }
    if (type == Type::Proc) return "<Proc>";
    if (type == Type::Lambda) return "<Lambda>";
    return "";
}

lisp::Env::Env(const std::vector<lisp::Cell>& args, const std::vector<lisp::Cell>& values, lisp::Env* super) : table(), super(super) {
    auto value = values.begin();

    for (auto& arg : args) table[arg.symbol] =* value++;
}

lisp::Cell& lisp::Env::Find(const std::string& name) {
    auto symbol = table.find(name);

    if (symbol != table.end()) {
        return symbol->second;
    }

    if (super == nullptr) {
        fprintf(stderr, "'%s' not defined in this scope", name.c_str());
        exit(1);
    }

    return super->Find(name);
}

lisp::Cell lisp::parse(const std::string& source) {
    auto tokens = lex(source.c_str());
    auto it = tokens.begin();

    return it != tokens.end() ? parse(it) : 0;
}

lisp::Cell lisp::parse(std::vector<token>::iterator& tokens) {
    auto tok = *tokens++;

    if (tok.type == '(') {
        std::vector<Cell> cells;

        while ((*tokens).type != ')') {
            cells.push_back(parse(tokens));
        }
        tokens++;
        return cells;
    }

    if (tok.type == 256) {
        return std::stoi(tok.buffer);
    }

    return tok.buffer;
}

lisp::Cell lisp::eval(Env* env, Cell cell) {
    if (cell.type == Cell::Type::Number) return cell;
    if (cell.type == Cell::Type::Symbol) return env->Find(cell.symbol);
    if (cell.type == Cell::Type::List) {
        if (cell.list[0].symbol == "quote") return cell.list[1];
        if (cell.list[0].symbol == "define") {
            return env->table[cell.list[1].symbol] = eval(env, cell.list[2]);
        }
        if (cell.list[0].symbol == "lambda") {
            cell.type = Cell::Lambda;
            return cell;
        }

        for (auto& arg : cell.list) arg = eval(env, arg);

        return cell.list[0](env, std::vector<Cell>(cell.list.begin() + 1, cell.list.end()));
    }
    return cell;
}

lisp::Cell lisp::eval(Env* env, const std::string& source) {
    return eval(env, parse(source));
}
