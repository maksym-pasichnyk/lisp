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

lisp::Cell lisp_ptr_to_int(lisp::Env* env, const lisp::List& args) {
    return *(int*)args[0].ptr;
}

lisp::Cell lisp_subscript(lisp::Env* env, const lisp::List& args) {
    if (args[1].type->is_pointer()) {
        auto type = args[1].type->remove_pointer();
        if (type->is_incomplete()) {
            fprintf(stderr, "lisp_subscript: bad type");
            exit(1);
        }
        return reinterpret_cast<char*>(args[1].ptr)[args[0].i32];
    }
    return args[1].list[args[0].i32];
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
    int result = (*arg++).i32;
    while (arg != args.end()) result += (*arg++).i32;
    return result;
}

lisp::Cell lisp_sub(lisp::Env* env, const lisp::List& args) {
    auto arg = args.begin();
    int result = (*arg++).i32;
    while (arg != args.end()) result -= (*arg++).i32;
    return result;
}

bool operator ==(const lisp::Cell& a, const lisp::Cell& b) {
    if (*a.type != *b.type) {
        fprintf(stderr, "%s and %s has different types\n", a.type->pretty_name().c_str(), b.type->pretty_name().c_str());
        exit(1);
    }

    if (a.type->is_integral()) {
        return a.i32 == b.i32;
    }

    if (*a.type == *get_type<std::string>()) {
        return a.s == b.s;
    }

    if (*a.type == *get_type<lisp::Symbol>()) {
        return a.s == b.s;
    }

    if (a.type->is_function()) {
        return a.proc == b.proc;
    }

    if (*a.type == *get_type<lisp::List>()) {
        return a.list == b.list;
    }

    return false;
}

bool operator >(const lisp::Cell& a, const lisp::Cell& b) {
    if (*a.type != *b.type) {
        fprintf(stderr, "%s and %s has different types\n", a.type->pretty_name().c_str(), b.type->pretty_name().c_str());
        exit(1);
    }

    if (a.type->is_integral()) {
        return a.i64 > b.i64;
    }

    if (*a.type == *get_type<std::string>()) {
        return a.s > b.s;
    }

    if (*a.type == *get_type<lisp::Symbol>()) {
        return a.s > b.s;
    }

    if (a.type->is_function()) {
        return a.proc > b.proc;
    }

    return false;
}

bool operator <(const lisp::Cell& a, const lisp::Cell& b) {
    if (*a.type != *b.type) {
        fprintf(stderr, "%s and %s has different types\n", a.type->pretty_name().c_str(), b.type->pretty_name().c_str());
        exit(1);
    }

    if (a.type->is_integral()) {
        return a.i64 < b.i64;
    }

    if (*a.type == *get_type<std::string>()) {
        return a.s < b.s;
    }

    if (*a.type == *get_type<lisp::Symbol>()) {
        return a.s < b.s;
    }

    if (a.type->is_function()) {
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
    return lisp::eval(env, args[0]).i32 ? lisp::eval(env, args[1]) : lisp::eval(env, args[2]);
}

lisp::Cell lisp_len(lisp::Env* env, const lisp::List& args) {
    return (int(args[0].list.size()));
}

lisp::Cell lisp_mul(lisp::Env* env, const lisp::List& args) {
    auto arg = args.begin();
    int result = (*arg++).i32;
    while (arg != args.end()) result *= (*arg++).i32;
    return (result);
}

lisp::Cell lisp_div(lisp::Env* env, const lisp::List& args) {
    auto arg = args.begin();
    int result = (*arg++).i32;
    while (arg != args.end()) result /= (*arg++).i32;
    return (result);
}

lisp::Cell lisp_ret(lisp::Env* env, const lisp::List& args) {
    return args.back();
}

lisp::Cell lisp_not(lisp::Env* env, const lisp::List& args) {
    auto a = args[0];

    if (a.type->is_scalar()) {
        return !a.b;
    }

    if (*a.type == *get_type<std::string>()) {
        return a.s.empty();
    }

    if (a.type->is_function()) {
        return !a.proc;
    }

    if (*a.type == *get_type<lisp::List>()) {
        return a.list.empty();
    }

    exit(1);
}

lisp::Cell lisp_define(lisp::Env* env, const lisp::List& args) {
    return env->table[args[0].s] = args[1];
}

lisp::Cell lisp_lambda(lisp::Env* env, const lisp::List& args) {
    lisp::Cell cell;
    cell.type = get_type<lisp::Lambda>();
    cell.list = { args[0], args[1] };
    return cell;
}

lisp::Cell lisp_while(lisp::Env* env, const lisp::List& args) {
    auto cond = args[0];
    auto body = args[1];

    while (lisp::eval(env, cond).i32) {
        lisp::eval(env, body);
    }

    return (void *) nullptr;
}

void import_core(lisp::Env& env) {
    env.table["print"] = lisp_print;
    env.table["malloc"] = lisp::CFunc(malloc);
    env.table["free"] = lisp::CFunc(free);
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
    env.table["null"] = (void*) nullptr;
    env.table["exit"] = lisp::CFunc(exit);
    env.table["ret"] = lisp_ret;

    lisp::eval(&env, "(def 'func inline (=> (name args body) (def name (=> args body))))");
    lisp::eval(&env, "(func 'map '(func lst) '(if (!= lst '()) '(begin(func ([] 0 lst))(map func (tail lst))) '()))");
    lisp::eval(&env, "(def 'foreach inline (=> (sym lst body) (map (=> '(sym) body) lst))))");
}