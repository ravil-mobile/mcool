cmake_minimum_required(VERSION 3.10)
project(mcool-test)

find_package(GTest REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GMOCK REQUIRED "gmock")

file(GLOB SCANNER_TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/scanner/*.cpp)
add_executable(scanner-tests ${SCANNER_TEST_SRC})
target_include_directories(scanner-tests PRIVATE ${GMOCK_INCLUDE_DIRS})
target_link_libraries(scanner-tests PRIVATE mcool-core GTest::gtest ${GMOCK_LINK_LIBRARIES})

file(GLOB PARSER_TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp)
add_executable(parser-tests ${PARSER_TEST_SRC})
target_include_directories(parser-tests PRIVATE ${GMOCK_INCLUDE_DIRS})
target_link_libraries(parser-tests PRIVATE mcool-core GTest::gtest ${GMOCK_LINK_LIBRARIES})

add_test(NAME scanner COMMAND scanner-tests)
add_test(NAME parser COMMAND parser-tests)
