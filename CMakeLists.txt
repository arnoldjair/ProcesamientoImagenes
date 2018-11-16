cmake_minimum_required(VERSION 2.8.12)
project(ProcesamientoImagenes)

add_definitions("-std=c++11")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(ProcesamientoImagenes main.cpp FilterFactory.cpp)
target_link_libraries(ProcesamientoImagenes ${CONAN_LIBS})
