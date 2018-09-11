//
//  WaitingTaskList_test.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/27/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>

#include <cppunit/extensions/HelperMacros.h>
#include <unistd.h>
#include <memory>
#include <atomic>
#include <thread>
#include "WaitingTask.h"
#include "WaitingTaskList.h"

class WaitingTaskList_test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(WaitingTaskList_test);
  CPPUNIT_TEST(addThenDone);
  CPPUNIT_TEST(doneThenAdd);
  CPPUNIT_TEST(stressTest);
  CPPUNIT_TEST_SUITE_END();
  
public:
  void addThenDone();
  void doneThenAdd();
  void stressTest();
  void setUp(){}
  void tearDown(){}
};

namespace  {
   class TestCalledTask : public demo::WaitingTask {
   public:
      TestCalledTask(std::atomic<bool>& iCalled): m_called(iCalled) {}

      void doSpawn() final {
         m_called = true;
      }
      
   private:
      std::atomic<bool>& m_called;
   };
   
   class TestValueSetTask : public demo::WaitingTask {
   public:
      TestValueSetTask(std::atomic<bool>& iValue): m_value(iValue) {}
         void doSpawn() final {
            CPPUNIT_ASSERT(m_value);
         }

      private:
         std::atomic<bool>& m_value;
   };
   
}

void WaitingTaskList_test::addThenDone()
{
   std::atomic<bool> called{false};
   
   demo::WaitingTaskList waitList;
   {
     demo::WaitingTaskHolder h(std::make_unique<TestCalledTask>(called) );

#pragma omp parallel default(shared)
      {
#pragma omp single
        {
          waitList.add(std::move(h));

          usleep(10);

          CPPUNIT_ASSERT(false==called);
   
          waitList.doneWaiting(std::exception_ptr{});
        }
      }
      CPPUNIT_ASSERT(true==called);
   }
   
   waitList.reset();
   called = false;
   
   {
     demo::WaitingTaskHolder h{std::make_unique<TestCalledTask>(called)};
   
#pragma omp parallel default(shared)
      {
#pragma omp single
        {
          waitList.add(std::move(h));

          usleep(10);
          CPPUNIT_ASSERT(false==called);
   
          waitList.doneWaiting(std::exception_ptr{});
        }
      }
      CPPUNIT_ASSERT(true==called);
   }
}

void WaitingTaskList_test::doneThenAdd()
{
   std::atomic<bool> called{false};
   demo::WaitingTaskList waitList;
   {
      demo::WaitingTaskHolder h{std::make_unique<TestCalledTask>(called)};

      waitList.doneWaiting(std::exception_ptr{});
#pragma omp parallel default(shared)
      {
#pragma omp single
        {  
          waitList.add(std::move(h));
        }
      }
      CPPUNIT_ASSERT(true==called);
   }
}

void WaitingTaskList_test::stressTest()
{
   std::atomic<bool> called{false};
   demo::WaitingTaskList waitList;
   
   unsigned int index = 1000;
   constexpr unsigned int nTasks = 10000;
   while(0 != --index) {
      called = false;
#pragma omp parallel default(shared)
      {
#pragma omp single
        {
          auto tLoop = demo::make_waiting_task(
           [&waitList,&called](std::exception_ptr const*){
            for(unsigned int i = 0; i<nTasks;++i) {
               demo::WaitingTaskHolder h{std::make_unique< TestCalledTask>(called)};
               waitList.add(std::move(h));
            }
         
            });
          demo::TaskBase::spawn(std::move(tLoop));
         
          auto doneWaitTask = demo::make_waiting_task([&waitList,&called](std::exception_ptr const*){
            called=true;
            waitList.doneWaiting(std::exception_ptr{});
            });
          demo::TaskBase::spawn(std::move(doneWaitTask));
        }
      } //end parallel
   }
}


CPPUNIT_TEST_SUITE_REGISTRATION( WaitingTaskList_test );
