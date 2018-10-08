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
  ${CMAKE_SOURCE_DIR}/CommonTestModules/BusyWaitPassFilter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/BusyWaitProducer.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesBusyWaitPassFilter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesBusyWaitParallelForPassFilter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesBusyWaitProducer.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesPassFilterBase.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesProducerBase.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesSleepingPassFilter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesSleepingParallelForPassFilter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesSleepingProducer.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesSyncGetPassFilterBase.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesSyncGetProducerBase.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesSyncGetSleepingPassFilter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/EventTimesSyncGetSleepingProducer.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/FastPassFilter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/FastProducer.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/PassFilter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/SleepingPassFilter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/SleepingProducer.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/ThreadSafeProducer.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/ThreadSafeWhichReadsProducer.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/ThreadSaferGetterFilter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/ThrowingFilter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/ThrowingProducer.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/Waiter.cpp
  ${CMAKE_SOURCE_DIR}/CommonTestModules/thread_type_from_config.cpp
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
  target_link_libraries(TBBRunUnitTests TBBFrameworkCore pthread ${TBB_LIBRARIES} ${CPPUNIT_LIBRARY})
endif()

add_executable(TBBDemo main.cpp)
target_link_libraries(TBBDemo TBBFrameworkCore TBBTestModules ${TBB_LIBRARY})
