cmake_minimum_required(VERSION 3.10)
project(mcool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WITH_TESTS "build with tests" OFF)

add_subdirectory(table-gen)

add_custom_command(
  COMMAND
  ${PROJECT_BINARY_DIR}/table-gen/ast/ast-tablegen
  --gen-ast-header
  -o=${PROJECT_BINARY_DIR}/generated/ast.h
  ${PROJECT_SOURCE_DIR}/src/Descriptions/ast.td
  WORKING_DIRECTORY
    ${PROJECT_BINARY_DIR}
  DEPENDS
    ast-tablegen
  OUTPUT
    ${PROJECT_BINARY_DIR}/generated/ast.h
  COMMENT "Generating AST header"
)

add_custom_command(
        COMMAND
        ${PROJECT_BINARY_DIR}/table-gen/ast/ast-tablegen
        --gen-visitor-header
        -o=${PROJECT_BINARY_DIR}/generated/visitor.h
        ${PROJECT_SOURCE_DIR}/src/Descriptions/ast.td
        WORKING_DIRECTORY
        ${PROJECT_BINARY_DIR}
        DEPENDS
        ast-tablegen
        OUTPUT
        ${PROJECT_BINARY_DIR}/generated/visitor.h
        COMMENT "Generating AST header"
)

add_custom_target(ast-codegen ALL DEPENDS
  ${PROJECT_BINARY_DIR}/generated/ast.h
  ${PROJECT_BINARY_DIR}/generated/visitor.h
)
add_dependencies(ast-codegen ast-tablegen)


include(FetchContent)
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG        v2.2.0
)
FetchContent_MakeAvailable(cli11)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/generated)

flex_target(scanner
            ${PROJECT_SOURCE_DIR}/src/Descriptions/scanner.flex
            ${PROJECT_BINARY_DIR}/generated/Scanner.cpp)

bison_target(parser
             ${PROJECT_SOURCE_DIR}/src/Descriptions/parser.bison
             ${PROJECT_BINARY_DIR}/generated/Parser.cpp
             DEFINES_FILE ${PROJECT_BINARY_DIR}/generated/Parser.h
             )

add_flex_bison_dependency(scanner parser)

set(MAIN_FILE ${PROJECT_SOURCE_DIR}/src/main.cpp)
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM SRC_FILES ${MAIN_FILE})

add_library(mcool-core
  ${SRC_FILES}
  ${FLEX_scanner_OUTPUTS}
  ${BISON_parser_OUTPUTS}
)

target_include_directories(mcool-core PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/common
  ${PROJECT_BINARY_DIR}/generated)

target_link_libraries(mcool-core PUBLIC
  CLI11::CLI11)

add_dependencies(mcool-core ast-codegen)

add_executable(${CMAKE_PROJECT_NAME} ${MAIN_FILE})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE mcool-core)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using LLVM include directory: ${LLVM_INCLUDE_DIRS}")

llvm_map_components_to_libnames(llvm_libs core support native mc mcparser)

target_include_directories(mcool-core PUBLIC ${LLVM_INCLUDE_DIRS})
target_link_libraries(mcool-core PUBLIC ${llvm_libs})
target_compile_definitions(mcool-core PUBLIC ${LLVM_DEFINITIONS})

if (WITH_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS ${CMAKE_PROJECT_NAME})