cmake_minimum_required(VERSION 3.13)
project(wet3)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(WHW3_Files/Code_Skeleton/Part1)
include_directories(WHW3_Files/Code_Skeleton/Part2)

add_executable(wet3
    WHW3_Files/Code_Skeleton/Part1/Headers.hpp
    WHW3_Files/Code_Skeleton/Part1/PCQueue.hpp
    WHW3_Files/Code_Skeleton/Part1/Semahore.cpp
    WHW3_Files/Code_Skeleton/Part1/Semaphore.hpp
    WHW3_Files/Code_Skeleton/Part2/Game.cpp
    WHW3_Files/Code_Skeleton/Part2/Game.hpp
    WHW3_Files/Code_Skeleton/Part2/main.cpp
    WHW3_Files/Code_Skeleton/Part2/Thread.hpp
    WHW3_Files/Code_Skeleton/Part2/utils.cpp
    WHW3_Files/Code_Skeleton/Part2/utils.hpp
    libpthreadGC2.a
    pthread.h
    pthreadVC2.lib)
