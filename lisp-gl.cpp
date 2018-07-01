#include <dlfcn.h>
#include "lisp-gl.h"

#include <GLFW/glfw3.h>

void lisp_glClearColor(int i1, int i2, int i3, int i4) {
    glClearColor(i1 / 255.0f, i2 / 255.0f, i3 / 255.0f, i4 / 255.0f);
}

void import_gl(lisp::Env& env) {
    env.table["GLFW_CONTEXT_VERSION_MAJOR"] = GLFW_CONTEXT_VERSION_MAJOR;
    env.table["GLFW_CONTEXT_VERSION_MINOR"] = GLFW_CONTEXT_VERSION_MINOR;
    env.table["GLFW_OPENGL_PROFILE"] = GLFW_OPENGL_PROFILE;
    env.table["GLFW_OPENGL_CORE_PROFILE"] = GLFW_OPENGL_CORE_PROFILE;
    env.table["GLFW_RESIZABLE"] = GLFW_RESIZABLE;

    env.table["glfwInit"] = lisp::CFunc(glfwInit);
    env.table["glfwWindowHint"] = lisp::CFunc(glfwWindowHint);
    env.table["glfwCreateWindow"] = lisp::CFunc(glfwCreateWindow);
    env.table["glfwMakeContextCurrent"] = lisp::CFunc(glfwMakeContextCurrent);
    env.table["glfwGetFramebufferSize"] = lisp::CFunc(glfwGetFramebufferSize);
    env.table["glfwWindowShouldClose"] = lisp::CFunc(glfwWindowShouldClose);
    env.table["glfwPollEvents"] = lisp::CFunc(glfwPollEvents);
    env.table["glfwSwapBuffers"] = lisp::CFunc(glfwSwapBuffers);
    env.table["glfwTerminate"] = lisp::CFunc(glfwTerminate);

    env.table["GL_FALSE"] = GL_FALSE;
    env.table["GL_COLOR_BUFFER_BIT"] = GL_COLOR_BUFFER_BIT;

    env.table["glClear"] = lisp::CFunc(glClear);
    env.table["glClearColor"] = lisp::CFunc(lisp_glClearColor);
    env.table["glViewport"] = lisp::CFunc(glViewport);
}