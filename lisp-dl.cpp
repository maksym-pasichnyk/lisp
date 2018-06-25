#include "lisp-dl.h"

#include <dlfcn.h>

lisp::Cell lisp_dlopen(lisp::Env*, const lisp::List& args) {
    return dlopen(args[0].text.c_str(), args[1].number);
}

lisp::Cell lisp_dlsym(lisp::Env*, const lisp::List& args) {
    return lisp::Proc(dlsym(args[0].ptr, args[1].text.c_str()));
}

void import_dl(lisp::Env& env) {
    env.table["dlopen"] = lisp_dlopen;
    env.table["dlsym"] = lisp_dlsym;
    env.table["RTLD_LAZY"] = RTLD_LAZY;
    env.table["RTLD_NOW"] = RTLD_NOW;
    env.table["RTLD_BINDING_MASK"] = RTLD_BINDING_MASK;
    env.table["RTLD_NOLOAD"] = RTLD_NOLOAD;
    env.table["RTLD_DEEPBIND"] = RTLD_DEEPBIND;
    env.table["RTLD_GLOBAL"] = RTLD_GLOBAL;
    env.table["RTLD_LOCAL"] = RTLD_LOCAL;
    env.table["RTLD_NODELETE"] = RTLD_NODELETE;
    env.table["RTLD_NEXT"] = RTLD_NEXT;
    env.table["RTLD_DEFAULT"] = RTLD_DEFAULT;

    lisp::eval(&env, "(func 'extern '(name symbol) '(def name (dlsym RTLD_DEFAULT symbol)))");
}