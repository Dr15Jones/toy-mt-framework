# See
# http://stackoverflow.com/questions/1027247/best-way-to-specify-sourcefiles-in-cmake
# including the discussion.
set(CORE_FILES
    SingleThreadedFrameworkCore/Event.cpp
    SingleThreadedFrameworkCore/EventProcessor.cpp
    SingleThreadedFrameworkCore/FactoryManagers.cpp
    SingleThreadedFrameworkCore/Filter.cpp
    SingleThreadedFrameworkCore/Getter.cpp
    SingleThreadedFrameworkCore/Module.cpp
    SingleThreadedFrameworkCore/Path.cpp
    SingleThreadedFrameworkCore/Producer.cpp
    SingleThreadedFrameworkCore/Schedule.cpp
    SingleThreadedFrameworkCore/Source.cpp
    SingleThreadedFrameworkCore/Waiter.cpp)

include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/SingleThreadedFrameworkCore)
add_library(SingleThreadedFrameworkCore ${CORE_FILES})

set(TEST_FILES
    SingleThreadedTestModules/BusyWaitPassFilter.cpp
    SingleThreadedTestModules/BusyWaitProducer.cpp
    SingleThreadedTestModules/EventTimesBusyWaitPassFilter.cpp
    SingleThreadedTestModules/EventTimesBusyWaitProducer.cpp
    SingleThreadedTestModules/EventTimesPassFilterBase.cpp
    SingleThreadedTestModules/EventTimesProducerBase.cpp
    SingleThreadedTestModules/EventTimesSleepingPassFilter.cpp
    SingleThreadedTestModules/EventTimesSleepingProducer.cpp
    SingleThreadedTestModules/PassFilter.cpp
    SingleThreadedTestModules/SleepingPassFilter.cpp
    SingleThreadedTestModules/SleepingProducer.cpp
    SingleThreadedTestModules/ThreadSaferGetterFilter.cpp
    ${CMAKE_SOURCE_DIR}/BusyWaitCalibration/busyWait.cpp
    ${CMAKE_SOURCE_DIR}/BusyWaitCalibration/busy_wait_scale_factor.cpp)

include_directories(BEFORE
    ${CMAKE_SOURCE_DIR}/BusyWaitCalibration)
add_library(SingleThreadedTestModules ${TEST_FILES} SingleThreadedFrameworkCore)

add_executable(SingleThreadedDemo main.cpp)
target_link_libraries(SingleThreadedDemo SingleThreadedFrameworkCore SingleThreadedTestModules)
