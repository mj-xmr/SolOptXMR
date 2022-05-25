cmake_minimum_required(VERSION 3.5.1)
project( opti-test )

file( GLOB SRCS src/*.cpp */*.h */*.hpp)

add_executable(${PROJECT_NAME} ${SRCS})
add_test(${PROJECT_NAME} ${PROJECT_NAME})
enjoSetupTarget(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} opti-lib test-common)
tsqSetupDirStatic(${PROJECT_NAME})
enjo_target_unity(${PROJECT_NAME})

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
