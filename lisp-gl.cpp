#include <dlfcn.h>
#include "lisp-gl.h"

#include <GLFW/glfw3.h>

#define dlsym_cast(handle, func) ((decltype(::func)*) dlsym(handle, #func))

static inline void* libGL() {
    static void* handle = dlopen("libGL.so", RTLD_NOW | RTLD_GLOBAL);
    return handle;
}

static inline void* libglfw() {
    static void* handle = dlopen("libglfw.so", RTLD_NOW | RTLD_GLOBAL);
    return handle;
}

lisp::Cell lisp_glfwInit(lisp::Env*, const lisp::List&) {
    static auto glfwInit = dlsym_cast(libglfw(), glfwInit);

    return glfwInit();
}

lisp::Cell lisp_glfwWindowHint(lisp::Env*, const lisp::List& args) {
    static auto glfwWindowHint = dlsym_cast(libglfw(), glfwWindowHint);

    glfwWindowHint(args[0].number, args[1].number);
    return (void*) nullptr;
}

lisp::Cell lisp_glfwCreateWindow(lisp::Env*, const lisp::List& args) {
    static auto glfwCreateWindow = dlsym_cast(libglfw(), glfwCreateWindow);

    return glfwCreateWindow(args[0].number, args[1].number, args[2].text.c_str(), (GLFWmonitor*) args[3].ptr, (GLFWwindow*) args[4].ptr);
}

lisp::Cell lisp_glfwMakeContextCurrent(lisp::Env*, const lisp::List& args) {
    static auto glfwMakeContextCurrent = dlsym_cast(libglfw(), glfwMakeContextCurrent);

    glfwMakeContextCurrent((GLFWwindow*) args[0].ptr);
    return (void*) nullptr;
}

lisp::Cell lisp_glfwGetFramebufferSize(lisp::Env*, const lisp::List& args) {
    static auto glfwGetFramebufferSize = dlsym_cast(libglfw(), glfwGetFramebufferSize);

    glfwGetFramebufferSize((GLFWwindow*) args[0].ptr, (int*) args[1].ptr, (int*) args[2].ptr);
}

lisp::Cell lisp_glfwWindowShouldClose(lisp::Env*, const lisp::List& args) {
    static auto glfwWindowShouldClose = dlsym_cast(libglfw(), glfwWindowShouldClose);

    return glfwWindowShouldClose((GLFWwindow*) args[0].ptr);
}

lisp::Cell lisp_glfwPollEvents(lisp::Env*, const lisp::List&) {
    static auto glfwPollEvents = dlsym_cast(libglfw(), glfwPollEvents);

    glfwPollEvents();
    return (void*) nullptr;
}

lisp::Cell lisp_glfwSwapBuffers(lisp::Env*, const lisp::List& args) {
    static auto glfwSwapBuffers = dlsym_cast(libglfw(), glfwSwapBuffers);

    glfwSwapBuffers((GLFWwindow*) args[0].ptr);
    return (void*) nullptr;
}

lisp::Cell lisp_glfwTerminate(lisp::Env*, const lisp::List&) {
    static auto glfwTerminate = dlsym_cast(libglfw(), glfwTerminate);

    glfwTerminate();
    return (void *) nullptr;
}

lisp::Cell lisp_glClear(lisp::Env*, const lisp::List& args) {
    static auto glClear = dlsym_cast(libGL(), glClear);

    glClear(static_cast<GLbitfield>(args[0].number));
    return (void *) nullptr;
}

lisp::Cell lisp_glClearColor(lisp::Env*, const lisp::List& args) {
    static auto glClearColor = dlsym_cast(libGL(), glClearColor);

    glClearColor(args[0].number / 255.0f, args[1].number / 255.0f, args[2].number / 255.0f, args[3].number / 255.0f);
    return (void *) nullptr;
}

lisp::Cell lisp_glViewport(lisp::Env*, const lisp::List& args) {
    static auto glViewport = dlsym_cast(libGL(), glViewport);

    glViewport(args[0].number, args[1].number, args[2].number, args[3].number);

    return (void *) nullptr;
}

void import_gl(lisp::Env& env) {
    env.table["GLFW_CONTEXT_VERSION_MAJOR"] = GLFW_CONTEXT_VERSION_MAJOR;
    env.table["GLFW_CONTEXT_VERSION_MINOR"] = GLFW_CONTEXT_VERSION_MINOR;
    env.table["GLFW_OPENGL_PROFILE"] = GLFW_OPENGL_PROFILE;
    env.table["GLFW_OPENGL_CORE_PROFILE"] = GLFW_OPENGL_CORE_PROFILE;
    env.table["GLFW_RESIZABLE"] = GLFW_RESIZABLE;

    env.table["glfwInit"] = lisp_glfwInit;
    env.table["glfwWindowHint"] = lisp_glfwWindowHint;
    env.table["glfwCreateWindow"] = lisp_glfwCreateWindow;
    env.table["glfwMakeContextCurrent"] = lisp_glfwMakeContextCurrent;
    env.table["glfwGetFramebufferSize"] = lisp_glfwGetFramebufferSize;
    env.table["glfwWindowShouldClose"] = lisp_glfwWindowShouldClose;
    env.table["glfwPollEvents"] = lisp_glfwPollEvents;
    env.table["glfwSwapBuffers"] = lisp_glfwSwapBuffers;
    env.table["glfwTerminate"] = lisp_glfwTerminate;

    env.table["GL_FALSE"] = GL_FALSE;
    env.table["GL_COLOR_BUFFER_BIT"] = GL_COLOR_BUFFER_BIT;

    env.table["glClear"] = lisp_glClear;
    env.table["glClearColor"] = lisp_glClearColor;

    env.table["glViewport"] = lisp_glViewport;
}