# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
cmake_minimum_required(VERSION 3.5.1)
#include(../../cmake/util.cmake)
project(opti-lib)

file( GLOB SRCS src/*.cpp */*.h */*.hpp)
#find_package(Lua REQUIRED) # TODO: For future

find_package(Boost QUIET REQUIRED COMPONENTS json) 

add_library (${PROJECT_NAME} ${LINK_TYPE} ${SRCS})
enjoSetupTarget(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} lib-base ${Boost_JSON_LIBRARY} ${LUA_LIBRARIES})

# TODO: Rather copy than symlink?
enjoSymlink(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/static ${CMAKE_BINARY_DIR}/../static)

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/bin/lib)
