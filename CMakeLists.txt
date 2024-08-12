cmake_minimum_required(VERSION 3.28)
project(server)

set(CMAKE_CXX_STANDARD 17)

# Add the executable target for your server project
add_executable(server main.cpp
        Server.cpp
        Server.h
        RoutingVector.cpp
        RoutingVector.h
        PostVector.cpp
        PostVector.h)

# Link with the WinSock2 library for networking
target_link_libraries(server ws2_32)