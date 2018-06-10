#include <iostream>
#include "lisp.h"

lisp::Cell proc_print(lisp::Env* env, lisp::Args args) {
    for (auto& arg : args) std::cout << arg.to_string() << ' ';
    std::cout << std::endl;
    return 0;
}

int main() {
    lisp::Env env({}, {}, nullptr);

    env.table["print"] = proc_print;

    std::string line;
    while (true) {
        std::cout << "lisp> ";
        std::getline(std::cin, line);

        if (line.empty()) break;

        lisp::eval(&env, line);
    }

    return 0;
}
