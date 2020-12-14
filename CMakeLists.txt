##################### Cmake Config #####################

cmake_minimum_required(VERSION 2.6)

project(Leaf-Examples)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -lpthread" CACHE STRING "")

##################### Leaf Include #####################

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

##################### Examples Source #####################

aux_source_directory(${CMAKE_SOURCE_DIR}/example/any EXAMPLE_ANY_SRC)

##################### Target option #####################

set(TARGET "example-any")

add_executable(
    ${TARGET} ${EXAMPLE_ANY_SRC}
)

target_link_libraries(
    ${TARGET}
    tbb 
)
