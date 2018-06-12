#include "lisp.h"
#include <sstream>
#include <dlfcn.h>

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
        if (input[0] == '\'') {
            out.push_back({'\'', "'"});
            input++;
        } else if (input[0] == '(') {
            out.push_back({'(', "("});
            input++;
        } else if (input[0] == ')') {
            out.push_back({')', ")"});
            input++;
        } else if (input[0] == '"') {
            const char* buf = ++input;
            while (input[0] != '"') ++input;
            out.push_back({258, std::string(buf, input++ - buf)});
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

lisp::Cell::Cell() : type(Type::Pointer), ptr(nullptr) {}
lisp::Cell::Cell(void* ptr) : type(Type::Pointer), ptr(ptr) {}
lisp::Cell::Cell(int number) : type(Type::Number), number(number)  {}
lisp::Cell::Cell(Symbol symbol) : type(Type::Symbol), symbol(std::move(symbol)) {}
lisp::Cell::Cell(std::string text) : type(Type::String), text(std::move(text)) {}
lisp::Cell::Cell(std::vector<Cell> list) : type(Type::List), list(std::move(list)) {}
lisp::Cell::Cell(Func proc) : type(Type::Procedure), proc(proc) {}

std::string lisp::Cell::to_string() const {
    if (type == Type::Pointer) {
        std::stringstream stream;
        stream << ptr;
        return stream.str();
    }
    if (type == Type::String) return text;
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
    if (type == Type::Procedure) return "<Proc>";
    if (type == Type::Lambda) return "<Lambda>";
    return "";
}

/*lisp::Env::Env(const std::vector<lisp::Cell>& args, const std::vector<lisp::Cell>& values, lisp::Env* super) : table(), super(super) {
    /*auto value = values.begin();

    for (auto& arg : args) table[arg.symbol] =* value++;
}*/

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

void lisp::Env::dump() {
    for (const auto&[key, val] : table) {
        printf("%s: %s\n", key.c_str(), val.to_string().c_str());
    }
    printf("\n");

    if (super) super->dump();
}

lisp::Cell lisp::parse(const std::string& source) {
    auto tokens = lex(source.c_str());
    auto it = tokens.begin();

    return it != tokens.end() ? parse(it) : (0);
}

lisp::Cell lisp::parse(std::vector<token>::iterator& tokens) {
    auto tok = *tokens++;

    if (tok.type == '\'') {
        Cell cell;
        cell.symbol = "quote";
        cell.type = Cell::Type ::Symbol;

        return std::vector<Cell>{cell, parse(tokens)};
    }

    if (tok.buffer == "#") {
        Cell cell;
        cell.symbol = "#";
        cell.type = Cell::Type ::Symbol;

        return std::vector<Cell>{cell, parse(tokens)};
    }

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

    if (tok.type == 257) {
        return (const Symbol&) tok.buffer;
    }

    if (tok.type == 258) {
        return tok.buffer;
    }
}

lisp::Cell lisp::eval(Env* env, Cell cell) {
    if (cell.type == Cell::Type::String) return cell;
    if (cell.type == Cell::Type::Number) return cell;
    if (cell.type == Cell::Type::Symbol) return env->Find(cell.symbol);
    if (cell.type == Cell::Type::List) {
        if (cell.list.empty()) return cell;

        if (cell.list[0].symbol == "#") {
            for (auto& arg : cell.list[1].list) {
                arg = eval(env, arg);
            }
            return cell.list[1];
        }

        if (cell.list[0].symbol == "...") {
            return Cell();
        }
        if (cell.list[0].symbol == "quote") {
            return cell.list[1];
        }
        if (cell.list[0].symbol == "define") {
            auto global = env;
            while (global->super) global = global->super;
            return global->table[cell.list[1].symbol] = eval(env, cell.list[2]);
        }
//        if (cell.list[0].symbol == "extern") {
//            return env->table[cell.list[1].symbol] = Func(dlsym(RTLD_DEFAULT, cell.list[2].symbol.c_str()));
//        }
        if (cell.list[0].symbol == "=>") {
            cell.env = new Env{{}, nullptr};
            cell.lambda = cell.env->unique++;
            cell.type = Cell::Type::Lambda;
            return cell;
        }
        if (cell.list[0].symbol == "begin") {
            Env context {{}, env};
            for (size_t i = 1; i < cell.list.size() - 1; ++i) {
                eval(&context, cell.list[i]);
            }
            return eval(&context, cell.list[cell.list.size() - 1]);
        }

        for (auto& arg : cell.list) {
            arg = eval(env, arg);
        }

        if (cell.list[0].type == Cell::Type::Procedure) {
            Env context {{}, env};
            return cell.list[0].proc(&context, std::vector<Cell>(cell.list.begin() + 1, cell.list.end()));
        }

        if (cell.list[0].type == Cell::Type::Lambda) {
            Env context;
            context.super = env;

            auto values = cell.list.begin() + 1;
            for (auto& arg : cell.list[0].list[1].list) {
                context.table[arg.symbol] = *values++;
            }

            return eval(&context, cell.list[0].list[2]);
        }

        printf("error: %s\n", cell.to_string().c_str());
        exit(1);
    }
    return cell;
}

lisp::Cell lisp::eval(Env* env, const std::string& source) {
    return eval(env, parse(source));
}
