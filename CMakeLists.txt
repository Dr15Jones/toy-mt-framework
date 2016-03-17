project(TOY-MT-FRAMEWORK)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(BusyWaitCalibration)
add_subdirectory(DispatchProcessingDemo)
add_subdirectory(SingleThreadedProcessingDemo)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    add_subdirectory(OpenMPProcessingDemo)
endif()


