cmake_minimum_required(VERSION 3.15)
project(Kaleidoscope)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    "include"
)

###########################################################
# Build main
###########################################################

set(SRC
    src/error.cpp
    src/lexer.cpp
    src/parser.cpp
)

add_executable(${PROJECT_NAME} main.cpp ${SRC})

target_include_directories(${PROJECT_NAME}
    PUBLIC "include")

###########################################################
# Build tests
###########################################################

set(test_depends
    ${SRC}
)

add_executable(test_lexer test/test_lexer.cpp ${test_depends})
#add_executable(test_parser test/test_parser.cpp ${test_depends})