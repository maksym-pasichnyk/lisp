#include <iostream>
#include "lisp.h"
#include <dlfcn.h>
#include <cstdarg>

lisp::Cell proc_print(lisp::Env* env, lisp::Args args) {
    for (auto& arg : args) std::cout << arg.to_string();
    std::cout << std::endl;
    return (void*)nullptr;
}

lisp::Cell proc_eval(lisp::Env* env, lisp::Args args) {
    return lisp::eval(env, args[0]);
}

lisp::Cell proc_new(lisp::Env* env, lisp::Args args) {
    return lisp::Func(malloc(size_t(args[0].number)));
}

lisp::Cell proc_free(lisp::Env* env, lisp::Args args) {
    free(args[0].ptr);
    return (void*)nullptr;
}

lisp::Cell proc_subscript(lisp::Env* env, lisp::Args args) {
    if (args[1].type == lisp::Cell::Type::Pointer) {
        return reinterpret_cast<char*>(args[1].ptr)[args[0].number];
    }
    return args[1].list[args[0].number];
}

lisp::Cell proc_dlsym(lisp::Env* env, lisp::Args args) {
    return lisp::Func(dlsym(args[0].ptr, args[1].text.c_str()));
}

lisp::Cell proc_front(lisp::Env* env, lisp::Args args) {
    return args[0].list.front();
}

lisp::Cell proc_back(lisp::Env* env, lisp::Args args) {
    return args[0].list.back();
}

lisp::Cell proc_tail(lisp::Env* env, lisp::Args args) {
    return std::vector<lisp::Cell>(args[0].list.begin() + 1, args[0].list.end());
}

lisp::Cell proc_add(lisp::Env* env, lisp::Args args) {
    auto arg = args.begin();
    int result = (*arg++).number;
    while (arg != args.end()) result += (*arg++).number;
    return result;
}

lisp::Cell proc_sub(lisp::Env* env, lisp::Args args) {
    auto arg = args.begin();
    int result = (*arg++).number;
    while (arg != args.end()) result -= (*arg++).number;
    return result;
}

bool operator ==(const lisp::Cell& a, const lisp::Cell& b) {
    if (a.type != b.type) return false;

    if (a.type == lisp::Cell::Type::Number) {
        return a.number == b.number;
    }

    if (a.type == lisp::Cell::Type::String) {
        return a.text == b.text;
    }

    if (a.type == lisp::Cell::Type::Symbol) {
        return a.symbol == b.symbol;
    }

    if (a.type == lisp::Cell::Type::Procedure) {
        return a.proc == b.proc;
    }

    if (a.type == lisp::Cell::Type::List) {
        return a.list == b.list;
    }

    if (a.type == lisp::Cell::Type::Lambda) {
        return (a.env == b.env) && (a.lambda == b.lambda);
    }

    return false;
}

bool operator >(const lisp::Cell& a, const lisp::Cell& b) {
    if (a.type != b.type) return false;

    if (a.type == lisp::Cell::Type::Number) {
        return a.number > b.number;
    }

    if (a.type == lisp::Cell::Type::String) {
        return a.text > b.text;
    }

    if (a.type == lisp::Cell::Type::Symbol) {
        return a.symbol > b.symbol;
    }

    if (a.type == lisp::Cell::Type::Procedure) {
        return a.proc > b.proc;
    }

    return false;
}

bool operator <(const lisp::Cell& a, const lisp::Cell& b) {
    if (a.type != b.type) return false;

    if (a.type == lisp::Cell::Type::Number) {
        return a.number < b.number;
    }

    if (a.type == lisp::Cell::Type::String) {
        return a.text < b.text;
    }

    if (a.type == lisp::Cell::Type::Symbol) {
        return a.symbol < b.symbol;
    }

    if (a.type == lisp::Cell::Type::Procedure) {
        return a.proc < b.proc;
    }

    return false;
}

lisp::Cell proc_eq(lisp::Env* env, lisp::Args args) {
    return args[0] == args[1];
}

lisp::Cell proc_ne(lisp::Env* env, lisp::Args args) {
    return !(args[0] == args[1]);
}

lisp::Cell proc_gt(lisp::Env* env, lisp::Args args) {
    return args[0] > args[1];
}

lisp::Cell proc_lt(lisp::Env* env, lisp::Args args) {
    return args[0] < args[1];
}

lisp::Cell proc_ge(lisp::Env* env, lisp::Args args) {
    return (args[0] == args[1]) || (args[0] > args[1]);
}

lisp::Cell proc_le(lisp::Env* env, lisp::Args args) {
    return ((args[0] == args[1]) || (args[0] < args[1]));
}

lisp::Cell proc_if(lisp::Env* env, lisp::Args args) {
    return lisp::eval(env, args[0]).number ? lisp::eval(env, args[1]) : lisp::eval(env, args[2]);
}

lisp::Cell proc_len(lisp::Env* env, lisp::Args args) {
    return (int(args[0].list.size()));
}

lisp::Cell proc_mul(lisp::Env* env, lisp::Args args) {
    auto arg = args.begin();
    int result = (*arg++).number;
    while (arg != args.end()) result *= (*arg++).number;
    return (result);
}

//lisp::Cell cpp_func(lisp::Env* env, lisp::Args args) {
//    return std::string("Hello from C++!");
//}
//
extern "C" {
    lisp::Cell c_func(lisp::Env* env, lisp::Args args) {
        return std::string("Hello from C!");
    }
}

int main() {
    lisp::Env env;

    env.table["print"] = proc_print;
    env.table["new"] = proc_new;
    env.table["free"] = proc_free;
    env.table["+"] = proc_add;
    env.table["-"] = proc_sub;
    env.table["*"] = proc_mul;
    env.table["front"] = proc_front;
    env.table["back"] = proc_back;
    env.table["tail"] = proc_tail;
    env.table["[]"] = proc_subscript;
    env.table["eval"] = proc_eval;
    env.table["=="] = proc_eq;
    env.table["!="] = proc_ne;
    env.table[">"] = proc_gt;
    env.table["<"] = proc_lt;
    env.table[">="] = proc_ge;
    env.table["<="] = proc_le;
    env.table["if"] = proc_if;
    env.table["len"] = proc_len;

    env.table["dlsym"] = proc_dlsym;
    env.table["RTLD_DEFAULT"] = RTLD_DEFAULT;

    env.table["false"] = 0;
    env.table["true"] = 1;
    env.table["null"] = (void*)nullptr;

    lisp::eval(&env, "(define func(=> (name args body)(eval #('define name(eval #('=> args body))))))");
    lisp::eval(&env, "(func 'def '(name val) '(eval #('define name val)))");
    lisp::eval(&env, "(func 'map '(func lst) '(if '(!= lst '()) '(begin((eval func) ([] 0 lst))(map func (tail lst))) null))");
    lisp::eval(&env, "(func 'for_each '(val list body) '(map #('=> #(val) body) list))");
    lisp::eval(&env, "(func 'extern '(name symbol) '(def name (dlsym RTLD_DEFAULT symbol)))");
    lisp::eval(&env, "(func 'let '(arg body) '(eval( ('=> (([] 0 arg)) body) ([] 1 arg))))");

    lisp::eval(&env, "(print)");

    lisp::eval(&env, R"((extern 'c_func "c_func"))");

    lisp::eval(&env, R"((map '(=> (a) (print a " * " a " = " (* a a))) '(10 11 12 13 14 15 16 17)))");
    lisp::eval(&env, "(print)");
    lisp::eval(&env, R"((for_each 'a '(10 11 12 13 14 15 16 17) '(print a " * " a " = " (* a a))))");
//    lisp::ev  al(&env, "(extern print proc_print)");
//    lisp::eval(&env, "(extern eval proc_eval)");
//    lisp::eval(&env, "(extern new proc_new)");
//    lisp::eval(&env, "(extern free proc_free)");
//    lisp::eval(&env, "(extern [] proc_subscript)");
//    lisp::eval(&env, "(extern c_func c_func)");
//    lisp::eval(&env, "(extern cpp_func _Z8cpp_funcPN4lisp3EnvERKSt6vectorINS_4CellESaIS3_EE)");
//    lisp::eval(&env, "(extern printf proc_printf)");
//
    lisp::eval(&env, "(print (c_func))");
//    lisp::eval(&env, "(print (cpp_func))");
//    lisp::eval(&env, "(define a (new 4))");
//    lisp::eval(&env, "(print a)");
//    lisp::eval(&env, "(print (quote a) (quote is) a)");
//    lisp::eval(&env, "(free a)");

    std::string line;
    while (true) {
        std::cout << "lisp> ";
        std::getline(std::cin, line);

        if (line.empty()) break;

        lisp::eval(&env, line);
    }

    return 0;
}

//lisp::Cell proc_sub(lisp::Env* env, lisp::Args args) {
//    auto arg = args.begin();
//    int result = (*arg++).number;
//    while (arg != args.end()) result -= (*arg++).number;
//    return result;
//}

//
//lisp::Cell proc_div(lisp::Env* env, lisp::Args args) {
//    auto arg = args.begin();
//    int result = (*arg++).number;
//    while (arg != args.end()) result /= (*arg++).number;
//    return result;
//}
//
//lisp::Cell proc_sqrt(lisp::Env* env, lisp::Args args) {
//    return sqrt(args[0].number);
//}
//
//lisp::Cell proc_set(lisp::Env* env, lisp::Args args) {
//    return env->Find(args[0].symbol) = args[1];
//}
