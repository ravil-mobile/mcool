cmake_minimum_required(VERSION 3.10)
project(mcool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(table-gen)

add_custom_command(
  COMMAND
  ${PROJECT_BINARY_DIR}/table-gen/ast/ast-tablegen
  --gen-ast-header
  -o=${PROJECT_BINARY_DIR}/generated/ast.h
  ${PROJECT_SOURCE_DIR}/descriptions/ast.td
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
        ${PROJECT_SOURCE_DIR}/descriptions/ast.td
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
            ${PROJECT_SOURCE_DIR}/descriptions/scanner.flex
            ${PROJECT_BINARY_DIR}/generated/scanner.cpp)

bison_target(parser
             ${PROJECT_SOURCE_DIR}/descriptions/parser.bison
             ${PROJECT_BINARY_DIR}/generated/parser.cpp
             DEFINES_FILE ${PROJECT_BINARY_DIR}/generated/parser.h
             )

add_flex_bison_dependency(scanner parser)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp) 

add_executable(${CMAKE_PROJECT_NAME}
  ${SRC_FILES}
  ${FLEX_scanner_OUTPUTS}
  ${BISON_parser_OUTPUTS}
  )

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/generated)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
  CLI11::CLI11)

add_dependencies(${CMAKE_PROJECT_NAME} ast-codegen)
