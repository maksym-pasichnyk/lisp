#include <stdint.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <dlfcn.h>

extern "C" {
void my_test() {
    void *libglfw = dlopen("libglfw.so", RTLD_NOW | RTLD_GLOBAL);
    auto glfwWindowHint = (decltype(::glfwWindowHint)*) dlsym(libglfw, "glfwWindowHint");

    glfwWindowHint(1, 1);
}
}