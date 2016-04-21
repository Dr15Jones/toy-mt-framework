add_definitions(-DPARALLEL_MODULES)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

set(CORE_FILES
    OpenMPFrameworkCore/Event.cpp
    OpenMPFrameworkCore/EventProcessor.cpp
    OpenMPFrameworkCore/FactoryManagers.cpp
    OpenMPFrameworkCore/Filter.cpp
    OpenMPFrameworkCore/FilterWrapper.cpp
    OpenMPFrameworkCore/Getter.cpp
    OpenMPFrameworkCore/Locks.cpp
    OpenMPFrameworkCore/Module.cpp
    OpenMPFrameworkCore/ModuleWrapper.cpp
    OpenMPFrameworkCore/Path.cpp
    OpenMPFrameworkCore/Producer.cpp
    OpenMPFrameworkCore/ProducerWrapper.cpp
    OpenMPFrameworkCore/Schedule.cpp
    OpenMPFrameworkCore/SimpleSource.cpp)

include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenMPFrameworkCore)
add_library(OpenMPFrameworkCore SHARED ${CORE_FILES})

set(TEST_FILES
    OpenMPTestModules/BusyWaitPassFilter.cpp
    OpenMPTestModules/BusyWaitProducer.cpp
    OpenMPTestModules/EventTimesBusyWaitPassFilter.cpp
    OpenMPTestModules/EventTimesBusyWaitProducer.cpp
    OpenMPTestModules/EventTimesPassFilterBase.cpp
    OpenMPTestModules/EventTimesProducerBase.cpp
    OpenMPTestModules/EventTimesSleepingPassFilter.cpp
    OpenMPTestModules/EventTimesSleepingProducer.cpp
    OpenMPTestModules/PassFilter.cpp
    OpenMPTestModules/SleepingPassFilter.cpp
    OpenMPTestModules/SleepingProducer.cpp
    OpenMPTestModules/ThreadSafeProducer.cpp
    OpenMPTestModules/ThreadSafeWhichReadsProducer.cpp
    OpenMPTestModules/ThreadSaferGetterFilter.cpp
    OpenMPTestModules/Waiter.cpp
    OpenMPTestModules/thread_type_from_config.cpp
    ${CMAKE_SOURCE_DIR}/BusyWaitCalibration/busyWait.cpp
    ${CMAKE_SOURCE_DIR}/BusyWaitCalibration/busy_wait_scale_factor.cpp)

include_directories(BEFORE
    ${CMAKE_SOURCE_DIR}/BusyWaitCalibration)
add_library(OpenMPTestModules SHARED ${TEST_FILES} OpenMPFrameworkCore)

add_executable(OpenMPDemo main.cpp)
target_link_libraries(OpenMPDemo OpenMPFrameworkCore OpenMPTestModules)
