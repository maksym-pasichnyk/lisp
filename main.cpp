#include <iostream>
#include "lisp.h"
#include <dlfcn.h>

typedef lisp::Cell(*Func)(lisp::Env*, lisp::Args);

extern "C" {
    lisp::Cell c_func(lisp::Env* env, lisp::Args args) {
        return std::string("Hello from C!");
    }
    lisp::Cell proc_print(lisp::Env* env, lisp::Args args) {
        for (auto& arg : args) std::cout << arg.to_string() << ' ';
        std::cout << std::endl;
        return 0;
    }

    lisp::Cell proc_eval(lisp::Env* env, lisp::Args args) {
        return lisp::eval(env, args[0]);
    }

    lisp::Cell proc_new(lisp::Env* env, lisp::Args args) {
        return malloc(size_t(args[0].number));
    }


    lisp::Cell proc_free(lisp::Env* env, lisp::Args args) {
        free(args[0].ptr);
        return 0;
    }

    lisp::Cell proc_subscript(lisp::Env* env, lisp::Args args) {
        if (args[1].type == lisp::Cell::Pointer) {
            return reinterpret_cast<char*>(args[1].ptr)[args[0].number];
        }
        return args[1].list[args[0].number];
    }

    lisp::Cell proc_extern(lisp::Env* env, lisp::Args args) {
        return Func(dlsym(RTLD_DEFAULT, args[0].symbol.c_str()));
    }
}

lisp::Cell cpp_func(lisp::Env* env, lisp::Args args) {
    return std::string("Hello from C++!");
}

int main() {
    lisp::Env env({}, {}, nullptr);

    env.table["extern"] = proc_extern;

    lisp::eval(&env, "(define print (extern (quote proc_print)))");
    lisp::eval(&env, "(define eval (extern (quote proc_eval)))");
    lisp::eval(&env, "(define new (extern (quote proc_new)))");
    lisp::eval(&env, "(define free (extern (quote proc_free)))");
    lisp::eval(&env, "(define [] (extern (quote proc_subscript)))");
    lisp::eval(&env, "(define c_func (extern (quote c_func)))");
    lisp::eval(&env, "(define cpp_func (extern (quote _Z8cpp_funcPN4lisp3EnvERKSt6vectorINS_4CellESaIS3_EE)))");

    lisp::eval(&env, "(print (c_func))");
    lisp::eval(&env, "(print (cpp_func))");
    lisp::eval(&env, "(free (new 123123))");

    std::string line;
    while (true) {
        std::cout << "lisp> ";
        std::getline(std::cin, line);

        if (line.empty()) break;

        lisp::eval(&env, line);
    }

    return 0;
}


//
//lisp::Cell proc_add(lisp::Env* env, lisp::Args args) {
//    auto arg = args.begin();
//    int result = (*arg++).number;
//    while (arg != args.end()) result += (*arg++).number;
//    return result;
//}
//
//lisp::Cell proc_sub(lisp::Env* env, lisp::Args args) {
//    auto arg = args.begin();
//    int result = (*arg++).number;
//    while (arg != args.end()) result -= (*arg++).number;
//    return result;
//}
//
//lisp::Cell proc_mul(lisp::Env* env, lisp::Args args) {
//    auto arg = args.begin();
//    int result = (*arg++).number;
//    while (arg != args.end()) result *= (*arg++).number;
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
