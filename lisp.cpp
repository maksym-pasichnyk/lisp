#include "lisp.h"
#include <sstream>

//static const unsigned char attr[256] = {
//    0,
//    1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
//    2, 2, 2, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 2, 5, 5, 5, 5, 5, 5, 5, 5,
//    5, 5, 5, 5, 5, 5, 5, 3, 3, 3,
//    3, 3, 3, 3, 3, 3, 3, 5, 5, 5,
//    5, 5, 5, 5, 4, 4, 4, 4, 4, 4,
//    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
//    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
//    5, 5, 5, 5, 5, 5, 4, 4, 4, 4,
//    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
//    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
//    4, 4, 5, 5, 5, 5, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//    1, 1, 1, 1, 1
//};
//
//static const unsigned char accept[6] = {
//    0,
//    0, 0, 3, 4, 5
//};
//
//static const char next[6][6] = {
//     0,  0,  0,  0,  0,  0,
//     0,  1,  2,  3,  4,  5,
//    -2, -2,  2, -2, -2, -2,
//    -3, -3, -3,  3, -3, -3,
//    -4, -4, -4, -4,  4, -4,
//    -5, -5, -5, -5, -5, -5
//};

struct token {
    int type;
    std::string buffer;
};

static std::vector<token> lex(const char* input) {
    std::vector<token> out;

//    while (*input) {
//        int state = 1;
//
//        const char* buf = input;
//        while ((state = next[state][attr[*input]]) > 0) {
//            input++;
//        }
//        state = -state;
//
//        switch (accept[state]) {
//            case 3: out.push_back({256, std::string(buf, input - buf)}); break;
//            case 4: out.push_back({257, std::string(buf, input - buf)}); break;
//            case 5: out.push_back({buf[0], std::string(buf, input - buf)}); break;
//            default: break;
//        }
//    }

    while (*input) {
        while (isspace(*input)) ++input;

        if (input[0] == '(') {
            out.push_back({'(', "("});
            input++;
        } else if (input[0] == ')') {
            out.push_back({')', ")"});
            input++;
        } else if (isalpha(input[0]) || (input[0] == '_')) {
            const char* buf = input++;
            while (isalnum(input[0]) || (input[0] == '_')) ++input;
            out.push_back({257, std::string(buf, input - buf)});
        } else if (isdigit(input[0]) || (input[0] == '-') && isdigit(input[1])) {
            const char* buf = input++;
            while (isdigit(input[0])) ++input;
            if (input[0] == '.') ++input;
            while (isdigit(input[0])) ++input;
            out.push_back({256, std::string(buf, input - buf)});
        } else if (ispunct(input[0])) {
            const char* buf = input++;
            while (ispunct(input[0]) && (input[0] != '(') && (input[0] != ')')) {
                ++input;
            }
            out.push_back({257, std::string(buf, input - buf)});
        } else ++input;
    }

    return out;
}

lisp::Cell::Cell(Cell::Type type) : type(Pointer), ptr(0) {}
lisp::Cell::Cell(void* ptr) : type(Pointer), ptr(ptr) {}
lisp::Cell::Cell(int number) : type(Number), number(number) {}
lisp::Cell::Cell(std::string symbol) : type(Symbol), symbol(std::move(symbol)) {}
lisp::Cell::Cell(std::vector<Cell> list) : type(List), list(std::move(list)) {}
lisp::Cell::Cell(Cell(*proc)(Env* , Args)) : type(Proc), proc(proc) {}

std::string lisp::Cell::to_string() const {
    if (type == Type::Pointer) {
        std::stringstream stream;
        stream << ptr;
        return stream.str();
    }
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
        if (cell.list[0].symbol == "begin") {
            auto context = new Env({}, {}, env);
            for (size_t i = 1; i < cell.list.size() - 1; ++i) {
                eval(env, cell.list[i]);
            }
            auto result = eval(env, cell.list[cell.list.size() - 1]);
            delete context;
            return result;
        }

        for (auto& arg : cell.list) arg = eval(env, arg);

        if (cell.list[0].type == Cell::Proc) {
            return cell.list[0].proc(env, std::vector<Cell>(cell.list.begin() + 1, cell.list.end()));
        }

        if (cell.list[0].type == Cell::Lambda) {
            auto context = new Env(cell.list[0].list[1].list, std::vector<Cell>(cell.list.begin() + 1, cell.list.end()), env);
            auto result = eval(context, cell.list[0].list[2]);
            delete context;
            return result;
        }
    }
    return cell;
}

lisp::Cell lisp::eval(Env* env, const std::string& source) {
    return eval(env, parse(source));
}
