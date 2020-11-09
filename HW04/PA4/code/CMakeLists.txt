cmake_minimum_required(VERSION 3.15)
project(code)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(code
        list.c
        list.h
        Makefile
        scheduler.c
        scheduler.h
        smp5_tests.c
        testrunner.c
        testrunner.h
        worker.c
        worker.h)
