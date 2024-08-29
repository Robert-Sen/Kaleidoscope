cmake_minimum_required(VERSION 3.15)
project(Kaleidoscope)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    "include"
)

###########################################################
# Dependences
###########################################################

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs core)

###########################################################
# Build main
###########################################################

set(SRC
    src/error.cpp
    src/lexer.cpp
    src/parser.cpp
    src/AST.cpp
)

add_executable(${PROJECT_NAME} main.cpp ${SRC})

target_include_directories(${PROJECT_NAME}
    PUBLIC "include")

target_link_libraries(${PROJECT_NAME} ${llvm_libs})

###########################################################
# Build tests
###########################################################

set(test_depends
    ${SRC}
)

add_executable(test_lexer test/test_lexer.cpp ${test_depends})

add_executable(test_parser test/test_parser.cpp ${test_depends})
target_link_libraries(test_parser ${llvm_libs})