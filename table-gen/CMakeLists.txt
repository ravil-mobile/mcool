cmake_minimum_required(VERSION 3.10)
project(custom-tblgen-backend)

cmake_policy(SET CMP0077 NEW)

find_package(LLVM REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})

include_directories(${LLVM_INCLUDE_DIRS})


find_program(LLVM_TABLEGEN_EXE "llvm-tblgen" ${LLVM_TOOLS_BINARY_DIR}
    NO_DEFAULT_PATH)

include(AddLLVM)
include(TableGen)
include(HandleLLVMOptions)
include(VersionFromVCS)
include(LLVMDistributionSupport)

add_subdirectory(ast)

set(CURR_DIR ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(
  OUTPUT ${CURR_DIR}/${CUSTOM_TABLEGEN_EXE}
  COMMAND ${CMAKE_COMMAND} -E copy ${CURR_DIR}/ast/${CUSTOM_TABLEGEN_EXE}
                                   ${CURR_DIR}/${CUSTOM_TABLEGEN_EXE}
  DEPENDS ${CURR_DIR}/ast/${CUSTOM_TABLEGEN_EXE}
)

add_custom_target(copy-binary ALL DEPENDS ${CURR_DIR}/${CUSTOM_TABLEGEN_EXE})
add_dependencies(copy-binary ${CUSTOM_TABLEGEN_TARGET})