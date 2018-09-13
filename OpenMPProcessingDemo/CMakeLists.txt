add_definitions(-DPARALLEL_MODULES)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

include_directories(${TBB_INCLUDE_DIRS})

set(CORE_FILES
    OpenMPFrameworkCore/Event.cpp
    OpenMPFrameworkCore/EventProcessor.cpp
    OpenMPFrameworkCore/FactoryManagers.cpp
    OpenMPFrameworkCore/Filter.cpp
    OpenMPFrameworkCore/FilterOnPathWrapper.cpp
    OpenMPFrameworkCore/FilterWrapper.cpp
    OpenMPFrameworkCore/Getter.cpp
    OpenMPFrameworkCore/Module.cpp
    OpenMPFrameworkCore/ModuleWrapper.cpp
    OpenMPFrameworkCore/Path.cpp
    OpenMPFrameworkCore/Producer.cpp
    OpenMPFrameworkCore/ProducerWrapper.cpp
    OpenMPFrameworkCore/Queues.cpp
    OpenMPFrameworkCore/Schedule.cpp
    OpenMPFrameworkCore/SerialTaskQueue.cpp    
    OpenMPFrameworkCore/SimpleSource.cpp
    OpenMPFrameworkCore/TimeMonitor.cpp
    OpenMPFrameworkCore/WaitingTaskList.cpp)

include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenMPFrameworkCore)
add_library(OpenMPFrameworkCore SHARED ${CORE_FILES})
target_link_libraries(OpenMPFrameworkCore ${TBB_LIBRARIES})

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

if(CPPUNIT_FOUND)
  include_directories(${CPPUNIT_INCLUDE_DIR})
  set(UNIT_TEST_FILES
      OpenMPUnitTests/Event_test.cpp
      OpenMPUnitTests/SerialTaskQueue_test.cpp
      OpenMPUnitTests/WaitingTaskList_test.cpp
      OpenMPUnitTests/main.cpp)
  add_executable(OpenMPRunUnitTests ${UNIT_TEST_FILES})
  target_link_libraries(OpenMPRunUnitTests OpenMPFrameworkCore ${TBB_LIBRARIES} ${CPPUNIT_LIBRARY})
endif()


add_executable(OpenMPDemo main.cpp)
target_link_libraries(OpenMPDemo OpenMPFrameworkCore OpenMPTestModules)
