project(TOY-MT-FRAMEWORK)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(BusyWaitCalibration)
add_subdirectory(DispatchProcessingDemo)
add_subdirectory(SingleThreadedProcessingDemo)

find_package(OpenMP)
if (OPENMP_FOUND)
    add_subdirectory(OpenMPProcessingDemo)
endif()


