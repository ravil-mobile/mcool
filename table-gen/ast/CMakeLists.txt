set(LLVM_LINK_COMPONENTS Support Object)

add_tablegen(ast-tablegen CUSTOM
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Misc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Generate.cpp
)

target_include_directories(ast-tablegen PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)