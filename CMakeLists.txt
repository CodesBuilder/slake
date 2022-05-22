cmake_minimum_required(VERSION 3.1)
project(slake VERSION 0.1.0)

set(C_STANDARD 99)

add_definitions(-D _CRT_SECURE_NO_WARNINGS -D _CRT_NONSTDC_NO_DEPRECATE)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR})

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

file(GLOB_RECURSE HAKE_SRC ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HAKE_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)

file(GLOB_RECURSE COMMON_HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)

BISON_TARGET(slake src/slake.y ${PROJECT_BINARY_DIR}/slake.tab.c COMPILE_FLAGS --verbose)
FLEX_TARGET(slake src/slake.l ${PROJECT_BINARY_DIR}/slake.l.c)
ADD_FLEX_BISON_DEPENDENCY(slake slake)

add_executable(slake ${BISON_slake_OUTPUTS} ${FLEX_slake_OUTPUTS} ${HAKE_SRC} ${HAKE_HEADERS} ${COMMON_HEADERS})
