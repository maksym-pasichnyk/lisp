cmake_minimum_required(VERSION 3.10)
project(lisp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
add_executable(lisp main.cpp lisp.h lisp.cpp lisp-core.cpp lisp-core.h lisp-gl.cpp lisp-gl.h lisp-dl.cpp lisp-dl.h cmake-build-debug/lisp-native.cpp cmake-build-debug/lisp-native.h lisp-jit.cpp lisp-jit.h)

target_link_libraries(lisp ${CMAKE_DL_LIBS} glfw GL)