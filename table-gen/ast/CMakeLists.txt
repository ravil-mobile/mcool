set(LLVM_LINK_COMPONENTS Support Object)

add_tablegen(ast-tablegen CUSTOM
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Misc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GeneratorAux.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/InheritanceBuilder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PrettyTreePrinter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AttributeBuilder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AstCodeEmitter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/VisitorCodeEmitter.cpp
)


# TODO: use `target_include_directories`
include_directories(ast-tablegen PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/common
)
