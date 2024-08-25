cmake_minimum_required(VERSION 3.15)
project(Kaleidoscope)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    "include"
)

###########################################################
# Build main
###########################################################

set(SRC main.cpp
    src/lexer.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME}
    PUBLIC "include")

###########################################################
# Build tests
###########################################################
add_executable(test_lexer test/test_lexer.cpp src/lexer.cpp)