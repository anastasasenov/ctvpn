cmake_minimum_required(VERSION 3.14)
project(ctvpn VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PNML_MODEL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/model.pnml")
set(XSLT_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ctvpn.xslt")
set(GENERATED_HEADER "${CMAKE_CURRENT_BINARY_DIR}/generated_net.hpp")

find_program(XSLTPROC_EXECUTABLE xsltproc REQUIRED)

add_custom_command(
    OUTPUT ${GENERATED_HEADER}
    COMMAND ${XSLTPROC_EXECUTABLE} ${XSLT_SCRIPT_FILE} ${PNML_MODEL_FILE} > ${GENERATED_HEADER}
    DEPENDS ${PNML_MODEL_FILE} ${XSLT_SCRIPT_FILE}
    COMMENT "[XSLT Processing] Re-compiling template header..."
    VERBATIM
)

add_executable(ctvpn  test.cpp ${GENERATED_HEADER})

target_include_directories(ctvpn PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ctvpn PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
