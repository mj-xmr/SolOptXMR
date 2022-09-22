cmake_minimum_required(VERSION 3.5.1)
include(../../externals/tsqsim/cmake/util.cmake)
project( opti-common-test )

file( GLOB SRCS src/*.cpp */*.h */*.hpp)

add_library (${PROJECT_NAME} ${LINK_TYPE} ${SRCS})
enjoSetupTarget(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} opti-lib test-common)
tsqSetupDirStatic(${PROJECT_NAME})
enjo_target_unity(${PROJECT_NAME})

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
