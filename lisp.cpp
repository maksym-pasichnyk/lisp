#include "lisp.h"
#include "lisp-core.h"
#include "lisp-dl.h"
#include "lisp-gl.h"
#include "stream.h"
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

std::vector<token> lex(const char* input) {
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
            uint8_t type = 0;
            const char* buf = input++;
            while (isdigit(input[0])) ++input;
            if (input[0] == '.') {
                type = 1;
                ++input;
            }
            while (isdigit(input[0])) ++input;
            if (tolower(input[0]) == 'f') {
                type = 2;
                ++input;
            }
            out.push_back({type ? (258 + type) : 256, std::string(buf, input - buf)});
        } else if (ispunct(input[0])) {
            const char* buf = input++;
            while (ispunct(input[0]) && (input[0] != '(') && (input[0] != ')') && (input[0] != '\'')) {
                ++input;
            }
            out.push_back({257, std::string(buf, input - buf)});
        } else ++input;
    }

    return out;
}

std::string lisp::Cell::to_string() const {
    std::string str;

    if (_quote_) {
        str.push_back('\'');
    }

    if (_variadic_) {
        str.append("...");
    }

    if (type->is_pointer()) {
        std::stringstream stream;
        stream << ptr;
        str.append(stream.str());
    }
    if (*type == *get_type<std::string>()) {
        str.append(s);
    }
    if (*type == *get_type<Symbol>()) {
        str.append(s);
    }
    if (type->is_integral()) {
        str.append(std::to_string(i64));
    }
    if (*type == *get_type<float>()) {
        str.append(std::to_string(f));
    }
    if (*type == *get_type<double>()) {
        str.append(std::to_string(d));
    }
    if (*type == *get_type<List>()) {
        str.append("(");
        if (!list.empty()) {
            for (const auto &e : list) {
                str.append(e.to_string()).push_back(' ');
            }
            str.pop_back();
        }
        str.append(")");
    }
    if (type->is_function()) {
        str.append("<Proc>");
    }
    if (*type == *get_type<Lambda>()) {
        str.append("(=> ");
        str.append(list[0].to_string());
        str.append(" ");
        str.append(list[1].to_string());
        str.append(")");
    }
    return str;
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

    return it != tokens.end() ? parse(it) :lisp::Cell (0);
}

lisp::Cell lisp::parse(std::vector<token>::iterator& tokens) {
    auto tok = *tokens++;

    if (tok.buffer == "inline") {
        Cell cell = parse(tokens);
        cell._inline_ = true;
        return cell;
    }

    if (tok.buffer == "...") {
        Cell cell = parse(tokens);
        cell._variadic_ = true;
        return cell;
    }

    if (tok.type == '\'') {
        Cell cell = parse(tokens);
        cell._quote_ = true;
        return cell;
    }

    if (tok.buffer == "::") {
        Cell cell = parse(tokens);
        cell._super_ = true;
        return cell;
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
        return Symbol(tok.buffer);
    }

    if (tok.type == 258) {
        return tok.buffer;
    }

    if (tok.type == 259) {
        return std::stod(tok.buffer);
    }

    if (tok.type == 260) {
        return std::stof(tok.buffer);
    }

    fprintf(stderr, "parse: wrong type\n");
    exit(1);
}

lisp::Cell lisp::apply(Env* env, Cell cell) {
    if (*cell.type == *get_type<Symbol>()) {
        auto symbol = env->table.find(cell.s);
        if (symbol != env->table.end()) {
            return symbol->second;
        }
    }
    if (*cell.type == *get_type<List>()) {
        for (auto &_cell_ : cell.list) {
            _cell_ = apply(env, _cell_);
        }
    }
    return cell;
}

lisp::Cell lisp::eval(Env* env, Cell cell) {
    if (cell._quote_) {
        cell._quote_ = false;
        return cell;
    }

    if (cell._super_) {
        cell._super_ = false;
        return eval(env->super ? env->super : env, cell);
    }

//    if (*cell.type == *get_type<std::string>()) return cell;
//    if (cell.type->is_integral()) return cell;
    if (*cell.type == *get_type<Symbol>()) {
        return env->Find(cell.s);
    }
    if (*cell.type == *get_type<List>()) {
        auto list = cell.list;

        if (list.empty()) return cell;

        if (list[0].s == "begin") {
            Env context {{}, env};
            for (size_t i = 1; i < list.size(); ++i) {
                eval(&context, list[i]);
            }
            return lisp::Cell();
        }

        auto first = eval(env, list[0]);

        if (*first.type == *get_type<CFunc>()) {
            std::vector<Argument> args;

            for (auto it = list.begin() + 1; it != list.end(); it++) {
                auto arg = cell._inline_ ? *it : eval(env, *it);

                auto type = arg.type;
                auto hash_code = type->hash_code();

                if (type->is_integral()) {
                    args.push_back(Argument::NewInt32(arg.i32));
                } else if (type->is_pointer()) {
                    args.push_back(Argument::NewPtr(arg.ptr));
                } else if (type->is_null_pointer()) {
                    args.push_back(Argument::NewPtr(0));
                } else if (hash_code == typeid(std::string).hash_code()) {
                    args.push_back(Argument::NewStr(arg.s));
                } else {
                    fprintf(stderr, "arg: %s\n", arg.to_string().c_str());
                    fprintf(stderr, "FuncPtr: argument with type '%s' not supported!\n", type->pretty_name().c_str());
                    exit(1);
                }
            }

            return first.func.invoke(args);
        }

        if (first.type->is_function()) {
            std::vector<Cell> args;

            for (auto arg = list.begin() + 1; arg != list.end(); arg++) {
                args.push_back(cell._inline_ ? *arg : eval(env, *arg));
            }

            Cell out = first.proc(env, args);
            out._inline_ = cell._inline_;
            return out;
        }

        if (*first.type == *get_type<Lambda>()) {
            Env context {{}, env};

            auto args = first.list[0];
            auto body = first.list[1];
            auto val = list.begin() + 1;
            auto vend = list.end();

            auto arg = args.list.begin();
            auto end = args.list.end();

            while (arg != end) {
                if ((*arg)._variadic_) {
                    std::vector<Cell> va_arg;
                    while (val != vend) {
                        va_arg.push_back(first._inline_ ? *val++ : eval(env, *val++));
                    }
                    context.table[(*arg++).s] = va_arg;
                    break;
                }

                context.table[(*arg++).s] = first._inline_ ? *val++ : eval(env, *val++);
            }

            return eval(first._inline_ ? env : &context, first._inline_ ? apply(&context, body) : body);
        }

        printf("error: %s '%s' is not callable\n", first.type->pretty_name().c_str(), list[0].to_string().c_str());
        exit(1);
    }
    return cell;
}

lisp::Cell lisp::eval(Env* env, const char* source) {
    return eval(env, parse(source));
}

lisp::Cell lisp::CFunc::invoke(const std::vector<Argument>& args) {
    return func->invoke(args);
}

lisp::Cell lisp::compile(const char* source) {
    stream<token> tokens = lex(source);
}

lisp::Cell lisp::compile(Env *env, Cell cell) {
    return 0;
}
