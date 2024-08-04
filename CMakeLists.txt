cmake_minimum_required(VERSION 3.29)
project(ash)

set(ARKIN_DEBUG true)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_C_FLAGS "-Wall -Wextra")

add_subdirectory("libs/arkin")

add_executable(${CMAKE_PROJECT_NAME}
    src/main.c
    src/lexer.c
    src/parser.c
)
target_link_libraries(${CMAKE_PROJECT_NAME} arkin)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
