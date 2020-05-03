cmake_minimum_required(VERSION 3.16)
project(shell)

set(CMAKE_CXX_STANDARD 14)

add_executable(shell main.cpp MiniShell.cpp MiniShell.h ShellKeyboard.cpp ShellKeyboard.h)