set(BUSYWAIT_FILES 
    busyWait.cpp
    busy_wait_scale_factor.cpp)
add_library(BusyWait SHARED ${BUSYWAIT_FILES})
add_executable(BusyWaitCalibrate main.cpp)
target_link_libraries(BusyWaitCalibrate BusyWait)
