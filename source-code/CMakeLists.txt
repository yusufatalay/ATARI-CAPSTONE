cmake_minimum_required(VERSION 3.23)
project(source_code C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(source_code
        basicFunctions.h
        constantFunctions.h
        definitions.h
        initializer.h
        main.c
        variableFunctions.h)
