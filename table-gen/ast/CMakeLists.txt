set(LLVM_LINK_COMPONENTS Support Object)

add_tablegen(ast-tablegen CUSTOM
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)