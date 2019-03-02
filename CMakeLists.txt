cmake_minimum_required(VERSION 3.9)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

project(prs CXX)

add_executable(prs 
    src/main.cpp
    src/parser.cpp
    src/parsestate.cpp
    src/tokens.cpp
    src/tracer.cpp
    )

target_link_libraries(prs
    -lasan
    )
    
target_include_directories(prs
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_compile_options(prs
    PUBLIC
        -Wall 
        -Wextra 
        -Werror=return-type
        -Werror=switch
        -Wfatal-errors
        -g
        -ggdb
        -fsanitize=address 
        -Werror
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wmisleading-indentation
        -Wduplicated-cond
        -Wlogical-op
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
    )
