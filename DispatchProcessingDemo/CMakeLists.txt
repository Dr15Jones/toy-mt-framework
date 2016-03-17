# See 
# http://stackoverflow.com/questions/1027247/best-way-to-specify-sourcefiles-in-cmake
# including the discussion.
set(CORE_FILES 
    DispatchFrameworkCore/Event.cpp
    DispatchFrameworkCore/EventProcessor.cpp
    DispatchFrameworkCore/FactoryManagers.cpp
    DispatchFrameworkCore/Filter.cpp
    DispatchFrameworkCore/FilterOnPathWrapper.cpp
    DispatchFrameworkCore/FilterWrapper.cpp
    DispatchFrameworkCore/Getter.cpp
    DispatchFrameworkCore/Module.cpp
    DispatchFrameworkCore/ModuleWrapper.cpp
    DispatchFrameworkCore/Path.cpp
    DispatchFrameworkCore/PrefetchAndWorkWrapper.cpp
    DispatchFrameworkCore/Producer.cpp
    DispatchFrameworkCore/ProducerWrapper.cpp
    DispatchFrameworkCore/Queues.cpp
    DispatchFrameworkCore/Schedule.cpp
    DispatchFrameworkCore/SimpleSource.cpp)

include_directories(BEFORE 
    ${CMAKE_CURRENT_SOURCE_DIR}/DispatchFrameworkCore)
add_library(DispatchFrameworkCore ${CORE_FILES})

set(TEST_FILES
    DispatchTestModules/BusyWaitPassFilter.cpp
    DispatchTestModules/BusyWaitProducer.cpp
    DispatchTestModules/EventTimesBusyWaitPassFilter.cpp
    DispatchTestModules/EventTimesBusyWaitProducer.cpp
    DispatchTestModules/EventTimesPassFilterBase.cpp
    DispatchTestModules/EventTimesProducerBase.cpp
    DispatchTestModules/EventTimesSleepingPassFilter.cpp
    DispatchTestModules/EventTimesSleepingProducer.cpp
    DispatchTestModules/EventTimesSyncGetPassFilterBase.cpp
    DispatchTestModules/EventTimesSyncGetProducerBase.cpp
    DispatchTestModules/EventTimesSyncGetSleepingPassFilter.cpp
    DispatchTestModules/EventTimesSyncGetSleepingProducer.cpp
    DispatchTestModules/PassFilter.cpp
    DispatchTestModules/SleepingPassFilter.cpp
    DispatchTestModules/SleepingProducer.cpp
    DispatchTestModules/ThreadSafeProducer.cpp
    DispatchTestModules/ThreadSafeWhichReadsProducer.cpp
    DispatchTestModules/ThreadSaferGetterFilter.cpp
    DispatchTestModules/Waiter.cpp
    DispatchTestModules/thread_type_from_config.cpp
    ${CMAKE_SOURCE_DIR}/BusyWaitCalibration/busyWait.cpp
    ${CMAKE_SOURCE_DIR}/BusyWaitCalibration/busy_wait_scale_factor.cpp)

include_directories(BEFORE 
    ${CMAKE_SOURCE_DIR}/BusyWaitCalibration)
add_library(DispatchTestModules ${TEST_FILES} DispatchFrameworkCore)

add_executable(DispatchDemo main.cpp)
target_link_libraries(DispatchDemo DispatchFrameworkCore DispatchTestModules)
    

