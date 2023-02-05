cmake_minimum_required(VERSION 3.10)
project(mcool-test)


find_package(GTest REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GMOCK REQUIRED "gmock")


add_library(tester INTERFACE IMPORTED)
target_link_libraries(tester INTERFACE mcool-core GTest::gtest ${GMOCK_LINK_LIBRARIES})
target_include_directories(tester INTERFACE ${GMOCK_INCLUDE_DIRS})


file(GLOB SCANNER_TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/scanner/*.cpp)
add_executable(scanner-tests ${SCANNER_TEST_SRC})
target_include_directories(scanner-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/scanner)
target_link_libraries(scanner-tests PRIVATE tester)


file(GLOB PARSER_TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp)
add_executable(parser-tests ${PARSER_TEST_SRC})
target_include_directories(parser-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/parser)
target_link_libraries(parser-tests PRIVATE tester)

file(GLOB SEMANT_TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/semant/*.cpp)
add_executable(semant-tests ${SEMANT_TEST_SRC})
target_include_directories(semant-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/semant)
target_link_libraries(semant-tests PRIVATE tester)

add_test(NAME scanner COMMAND scanner-tests)
add_test(NAME parser COMMAND parser-tests)
add_test(NAME semant COMMAND semant-tests)

