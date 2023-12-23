cmake_minimum_required(VERSION 3.20)
project(lab5)

add_definitions( -Wall -Werror -Wextra)
add_executable(control controlNode.cpp)
add_executable(calculationNode calculationNode.cpp)
target_link_libraries(control zmq)
target_link_libraries(calculationNode zmq)