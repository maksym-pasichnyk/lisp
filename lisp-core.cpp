#include "lisp-core.h"

#include <iostream>

lisp::Cell lisp_print(lisp::Env* env, const lisp::List& args) {
    for (auto& arg : args) std::cout << arg.to_string();
    std::cout << std::endl;
    return (void*)nullptr;
}

lisp::Cell lisp_eval(lisp::Env* env, const lisp::List& args) {
    return lisp::eval(env, args[0]);
}

lisp::Cell lisp_new(lisp::Env* env, const lisp::List& args) {
    return malloc(size_t(args[0].number));
}

lisp::Cell lisp_free(lisp::Env* env, const lisp::List& args) {
    free(args[0].ptr);
    return (void*)nullptr;
}

lisp::Cell lisp_ptr_to_int(lisp::Env* env, const lisp::List& args) {
    return *(int*)args[0].ptr;
}

lisp::Cell lisp_subscript(lisp::Env* env, const lisp::List& args) {
    if (args[1].type == lisp::Type::Ptr) {
        return reinterpret_cast<char*>(args[1].ptr)[args[0].number];
    }
    return args[1].list[args[0].number];
}

lisp::Cell lisp_front(lisp::Env* env, const lisp::List& args) {
    return args[0].list.front();
}

lisp::Cell lisp_back(lisp::Env* env, const lisp::List& args) {
    return args[0].list.back();
}

lisp::Cell lisp_tail(lisp::Env* env, const lisp::List& args) {
    return std::vector<lisp::Cell>(args[0].list.begin() + 1, args[0].list.end());
}

lisp::Cell lisp_add(lisp::Env* env, const lisp::List& args) {
    auto arg = args.begin();
    int result = (*arg++).number;
    while (arg != args.end()) result += (*arg++).number;
    return result;
}

lisp::Cell lisp_sub(lisp::Env* env, const lisp::List& args) {
    auto arg = args.begin();
    int result = (*arg++).number;
    while (arg != args.end()) result -= (*arg++).number;
    return result;
}

bool operator ==(const lisp::Cell& a, const lisp::Cell& b) {
    if (a.type != b.type) {
        fprintf(stderr, "%s and %s has different types\n", a.to_string().c_str(), b.to_string().c_str());
        exit(1);
    }

    if (a.type == lisp::Type::Number) {
        return a.number == b.number;
    }

    if (a.type == lisp::Type::String) {
        return a.text == b.text;
    }

    if (a.type == lisp::Type::Symbol) {
        return a.symbol == b.symbol;
    }

    if (a.type == lisp::Type::Proc) {
        return a.proc == b.proc;
    }

    if (a.type == lisp::Type::List) {
        return a.list == b.list;
    }

    return false;
}

bool operator >(const lisp::Cell& a, const lisp::Cell& b) {
    if (a.type != b.type) {
        fprintf(stderr, "%s and %s has different types\n", a.to_string().c_str(), b.to_string().c_str());
        exit(1);
    }

    if (a.type == lisp::Type::Number) {
        return a.number > b.number;
    }

    if (a.type == lisp::Type::String) {
        return a.text > b.text;
    }

    if (a.type == lisp::Type::Symbol) {
        return a.symbol > b.symbol;
    }

    if (a.type == lisp::Type::Proc) {
        return a.proc > b.proc;
    }

    return false;
}

bool operator <(const lisp::Cell& a, const lisp::Cell& b) {
    if (a.type != b.type) {
        fprintf(stderr, "%s and %s has different types\n", a.to_string().c_str(), b.to_string().c_str());
        exit(1);
    }

    if (a.type == lisp::Type::Number) {
        return a.number < b.number;
    }

    if (a.type == lisp::Type::String) {
        return a.text < b.text;
    }

    if (a.type == lisp::Type::Symbol) {
        return a.symbol < b.symbol;
    }

    if (a.type == lisp::Type::Proc) {
        return a.proc < b.proc;
    }

    return false;
}

lisp::Cell lisp_eq(lisp::Env* env, const lisp::List& args) {
    return args[0] == args[1];
}

lisp::Cell lisp_ne(lisp::Env* env, const lisp::List& args) {
    return !(args[0] == args[1]);
}

lisp::Cell lisp_gt(lisp::Env* env, const lisp::List& args) {
    return args[0] > args[1];
}

lisp::Cell lisp_lt(lisp::Env* env, const lisp::List& args) {
    return args[0] < args[1];
}

lisp::Cell lisp_ge(lisp::Env* env, const lisp::List& args) {
    return (args[0] == args[1]) || (args[0] > args[1]);
}

lisp::Cell lisp_le(lisp::Env* env, const lisp::List& args) {
    return ((args[0] == args[1]) || (args[0] < args[1]));
}

lisp::Cell lisp_if(lisp::Env* env, const lisp::List& args) {
    return lisp::eval(env, args[0]).number ? lisp::eval(env, args[1]) : lisp::eval(env, args[2]);
}

lisp::Cell lisp_len(lisp::Env* env, const lisp::List& args) {
    return (int(args[0].list.size()));
}

lisp::Cell lisp_mul(lisp::Env* env, const lisp::List& args) {
    auto arg = args.begin();
    int result = (*arg++).number;
    while (arg != args.end()) result *= (*arg++).number;
    return (result);
}

lisp::Cell lisp_div(lisp::Env* env, const lisp::List& args) {
    auto arg = args.begin();
    int result = (*arg++).number;
    while (arg != args.end()) result /= (*arg++).number;
    return (result);
}

lisp::Cell lisp_not(lisp::Env* env, const lisp::List& args) {
    auto a = args[0];

    if (a.type == lisp::Type::Number) {
        return !a.number;
    }

    if (a.type == lisp::Type::String) {
        return a.text.empty();
    }

    if (a.type == lisp::Type::Proc) {
        return !a.proc;
    }

    if (a.type == lisp::Type::List) {
        return a.list.empty();
    }

    exit(1);
}

lisp::Cell lisp_define(lisp::Env* env, const lisp::List& args) {
    return env->table[args[0].symbol] = args[1];
}

lisp::Cell lisp_lambda(lisp::Env* env, const lisp::List& args) {
    lisp::Cell cell;
    cell.type = lisp::Type::Lambda;
    cell.list = { args[0], args[1] };
    return cell;
}

lisp::Cell lisp_while(lisp::Env* env, const lisp::List& args) {
    auto cond = args[0];
    auto body = args[1];

    while (lisp::eval(env, cond).number) {
        lisp::eval(env, body);
    }

    return (void *) nullptr;
}

lisp::Cell lisp_exit(lisp::Env* env, const lisp::List& args) {
    exit(args[0].number);
    return (void *) nullptr;
}

void import_core(lisp::Env& env) {
    env.table["print"] = lisp_print;
    env.table["new"] = lisp_new;
    env.table["free"] = lisp_free;
    env.table["ptr_to_int"] = lisp_ptr_to_int;
    env.table["+"] = lisp_add;
    env.table["-"] = lisp_sub;
    env.table["*"] = lisp_mul;
    env.table["/"] = lisp_div;
    env.table["!"] = lisp_not;
    env.table["front"] = lisp_front;
    env.table["back"] = lisp_back;
    env.table["tail"] = lisp_tail;
    env.table["[]"] = lisp_subscript;
    env.table["eval"] = lisp_eval;
    env.table["=="] = lisp_eq;
    env.table["!="] = lisp_ne;
    env.table[">"] = lisp_gt;
    env.table["<"] = lisp_lt;
    env.table[">="] = lisp_ge;
    env.table["<="] = lisp_le;
    env.table["if"] = lisp_if;
    env.table["while"] = lisp_while;
    env.table["len"] = lisp_len;
    env.table["def"] = lisp_define;
    env.table["=>"] = lisp_lambda;
    env.table["false"] = false;
    env.table["true"] = true;
    env.table["null"] = (void*)nullptr;
    env.table["exit"] = lisp_exit;

    lisp::eval(&env, "(def 'func inline (=> (name args body) (def name (=> args body))))");
    lisp::eval(&env, "(func 'map '(func lst) '(if (!= lst '()) '(begin(func ([] 0 lst))(map func (tail lst))) '()))");
    lisp::eval(&env, "(def 'foreach inline (=> (sym lst body) (map (=> '(sym) body) lst))))");
}