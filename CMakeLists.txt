cmake_minimum_required(VERSION 3.16)
project(UniCare)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(UniCare
        main.cpp
        Appointment.cpp
        Appointment.h
        Medicine.cpp
        Medicine.h
        em.cpp
        em.h)

set_target_properties(UniCare PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
