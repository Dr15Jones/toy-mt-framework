# See
# http://stackoverflow.com/questions/1027247/best-way-to-specify-sourcefiles-in-cmake
# including the discussion.
include_directories(${TBB_INCLUDE_DIRS})

set(CORE_FILES
    TBBFrameworkCore/Event.cpp
    TBBFrameworkCore/EventProcessor.cpp
    TBBFrameworkCore/FactoryManagers.cpp
    TBBFrameworkCore/Filter.cpp
    TBBFrameworkCore/FilterOnPathWrapper.cpp
    TBBFrameworkCore/FilterWrapper.cpp
    TBBFrameworkCore/Getter.cpp
    TBBFrameworkCore/Module.cpp
    TBBFrameworkCore/ModuleThreadStack.cpp
    TBBFrameworkCore/ModuleWrapper.cpp
    TBBFrameworkCore/Path.cpp
    TBBFrameworkCore/PrefetchAndWorkWrapper.cpp
    TBBFrameworkCore/Producer.cpp
    TBBFrameworkCore/ProducerWrapper.cpp
    TBBFrameworkCore/Queues.cpp
    TBBFrameworkCore/Schedule.cpp
    TBBFrameworkCore/SerialTaskQueue.cpp
    TBBFrameworkCore/SimpleSource.cpp
    TBBFrameworkCore/WaitingList.cpp
    TBBFrameworkCore/WaitingTaskList.cpp)

include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/TBBFrameworkCore)
add_library(TBBFrameworkCore SHARED ${CORE_FILES})
target_link_libraries(TBBFrameworkCore ${TBB_LIBRARIES})

set(TEST_FILES
  TBBTestModules/BusyWaitPassFilter.cpp
  TBBTestModules/BusyWaitProducer.cpp
  TBBTestModules/EventTimesBusyWaitPassFilter.cpp
  TBBTestModules/EventTimesBusyWaitProducer.cpp
  TBBTestModules/EventTimesPassFilterBase.cpp
  TBBTestModules/EventTimesProducerBase.cpp
  TBBTestModules/EventTimesSleepingPassFilter.cpp
  TBBTestModules/EventTimesSleepingProducer.cpp
  TBBTestModules/EventTimesSyncGetPassFilterBase.cpp
  TBBTestModules/EventTimesSyncGetProducerBase.cpp
  TBBTestModules/EventTimesSyncGetSleepingPassFilter.cpp
  TBBTestModules/EventTimesSyncGetSleepingProducer.cpp
  TBBTestModules/FastPassFilter.cpp
  TBBTestModules/FastProducer.cpp
  TBBTestModules/GeantVProducer.cpp
  TBBTestModules/PassFilter.cpp
  TBBTestModules/SleepingPassFilter.cpp
  TBBTestModules/SleepingProducer.cpp
  TBBTestModules/ThreadSafeProducer.cpp
  TBBTestModules/ThreadSafeWhichReadsProducer.cpp
  TBBTestModules/ThreadSaferGetterFilter.cpp
  TBBTestModules/ThrowingFilter.cpp
  TBBTestModules/ThrowingProducer.cpp
  TBBTestModules/Waiter.cpp
  TBBTestModules/thread_type_from_config.cpp
  ${CMAKE_SOURCE_DIR}/BusyWaitCalibration/busyWait.cpp
  ${CMAKE_SOURCE_DIR}/BusyWaitCalibration/busy_wait_scale_factor.cpp)

include_directories(BEFORE
    ${CMAKE_SOURCE_DIR}/BusyWaitCalibration)
add_library(TBBTestModules SHARED ${TEST_FILES})
target_link_libraries(TBBTestModules TBBFrameworkCore)

if(CPPUNIT_FOUND)
  include_directories(${CPPUNIT_INCLUDE_DIR})
  set(UNIT_TEST_FILES
      TBBUnitTests/Event_test.cpp
      TBBUnitTests/SerialTaskQueue_test.cpp
      TBBUnitTests/WaitingTaskList_test.cpp
      TBBUnitTests/main.cpp)
  add_executable(TBBRunUnitTests ${UNIT_TEST_FILES})
  target_link_libraries(TBBRunUnitTests ${TBB_LIBRARIES} ${CPPUNIT_LIBRARY})
endif()

add_executable(TBBDemo main.cpp)
target_link_libraries(TBBDemo TBBFrameworkCore TBBTestModules ${TBB_LIBRARY})
