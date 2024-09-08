cmake_minimum_required(VERSION 3.29)
project(part-3-chapter-13)

set(CMAKE_CXX_STANDARD 20)

add_executable(part-3-13-1 13-1-No-Thread-Safety.cpp)
add_executable(part-3-13-2 13-2-Critical-Section.cpp)
add_executable(part-3-13-3 13-3-Mutex.cpp)
add_executable(part-3-13-4 13-4-Semaphore.cpp)
add_executable(part-3-13-5 13-5-NamedMutex.cpp)
add_executable(part-3-13-6 13-6-Mutex-Wait-Abandoned-State.cpp)
