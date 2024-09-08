cmake_minimum_required(VERSION 3.29)
project(part-3-chapter-14)

set(CMAKE_CXX_STANDARD 20)

add_executable(part-3-14-1 14-1-Event-Object.cpp)
add_executable(part-3-14-2 14-2-Event-Object-Manual-Reset-Mode.cpp)
add_executable(part-3-14-3 14-3-Event-With-Mutex.cpp)
add_executable(part-3-14-4 14-4-Manual-Reset-Timer.cpp)
add_executable(part-3-14-4-1 14-4-1-Manual-Reset-Timer-Custom-Experiment.cpp)
add_executable(part-3-14-5 14-5-Manual-Reset-Periodic-Timer.cpp)
