project(TOY_MT_FRAMEWORK)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
find_package(OpenMP)
find_package(TBB)
find_package(CppUnit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(BusyWaitCalibration)
add_subdirectory(DispatchProcessingDemo)
add_subdirectory(SingleThreadedProcessingDemo)
if(TBB_FOUND)
    add_subdirectory(TBBProcessingDemo)
endif()
if (OPENMP_FOUND)
    add_subdirectory(OpenMPProcessingDemo)
endif()
