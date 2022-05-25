cmake_minimum_required(VERSION 3.5.1)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
#include(util)

project (SolOptXMR)
enable_testing()

option(BUILD_TESTS "Build Tests"   ON)
option(VERBOSE_FUNCTIONS "Verbose functions"   OFF)
if (VERBOSE_FUNCTIONS)
	add_compile_definitions(VERBOSE_FUNCTIONS)
endif()


add_subdirectory(externals)
add_subdirectory(src)

if (BUILD_TESTS)
	add_subdirectory(tests)
endif()
