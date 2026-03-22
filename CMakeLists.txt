cmake_minimum_required(VERSION 3.28)
project(server)

set(CMAKE_CXX_STANDARD 17)

# Add the executable target for your server project
add_executable(server main.cpp
        Server/Server.cpp
        Server/Server.h
        Server/RoutingVector.cpp
        Server/RoutingVector.h
        Server/PostVector.cpp
        Server/PostVector.h
        Server/Database/Database.cpp
        Server/Database/Database.h
        Server/Database/Table.cpp
        Server/Database/Table.h
        Server/Database/Row.cpp
        Server/Database/Row.h
        Server/SessionVariable.cpp
        Server/SessionVariable.h
        functions.h)

# Link with the WinSock2 library for networking
target_link_libraries(server ws2_32)