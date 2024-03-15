cmake_minimum_required(VERSION 3.17)
project(MultiClient C)

set(CMAKE_C_STANDARD 11)

add_executable(server server.c)
add_executable(client client.c)

# Link against the ws2_32.lib library for Windows socket programming
target_link_libraries(server wsock32 ws2_32)
target_link_libraries(client wsock32 ws2_32)
