#include "lisp-dl.h"

#include <dlfcn.h>

void import_dl(lisp::Env& env) {
    env.table["dlopen"] = lisp::CFunc(dlopen);
    env.table["dlsym"] = lisp::CFunc(dlsym);
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
}