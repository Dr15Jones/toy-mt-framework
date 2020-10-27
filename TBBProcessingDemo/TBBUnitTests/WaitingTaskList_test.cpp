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
#include "tbb/task_group.h"
#include "WaitingTaskList.h"
#include "WaitingTask.h"
#include "WaitingTaskHolder.h"

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
#define CXX_THREAD_AVAILABLE
#endif

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
      TestCalledTask(std::atomic<bool>& iCalled, std::atomic<int>& iCount): 
	m_called(iCalled),
	m_count(iCount){++m_count;}

      void execute() override {
         m_called = true;
	 --m_count;
      }
      
   private:
      std::atomic<bool>& m_called;
      std::atomic<int>& m_count;
   };
   
  class TestValueSetTask : public demo::WaitingTask {
   public:
      TestValueSetTask(std::atomic<bool>& iValue): m_value(iValue) {}
         void execute() override {
            CPPUNIT_ASSERT(m_value);
         }

      private:
         std::atomic<bool>& m_value;
   };
   
}

void WaitingTaskList_test::addThenDone()
{
   std::atomic<bool> called{false};
   std::atomic<int> count{0};

   demo::WaitingTaskList waitList;
   {
      tbb::task_group group;
      auto t = new TestCalledTask{called,count};
   
      waitList.add(demo::WaitingTaskHolder(group,t));

      usleep(10);
      __sync_synchronize();
      CPPUNIT_ASSERT(false==called);
   
      waitList.doneWaiting(std::exception_ptr{});

      group.wait();
      __sync_synchronize();
      CPPUNIT_ASSERT(true==called);
   }
   
   waitList.reset();
   called = false;
   
   {
      tbb::task_group group;
   
      auto t = new TestCalledTask{called,count};
   
      waitList.add(demo::WaitingTaskHolder(group,t));

      usleep(10);
      CPPUNIT_ASSERT(false==called);
   
      waitList.doneWaiting(std::exception_ptr{});
      group.wait();
      CPPUNIT_ASSERT(true==called);
   }
}

void WaitingTaskList_test::doneThenAdd()
{
   std::atomic<bool> called{false};
   std::atomic<int> count{0};
   demo::WaitingTaskList waitList;
   {
      tbb::task_group group;
      auto t = new TestCalledTask{called,count};
      
      waitList.doneWaiting(std::exception_ptr{});
   
      waitList.add(demo::WaitingTaskHolder(group,t));
      group.wait();
      CPPUNIT_ASSERT(true==called);
   }
}

namespace {
#if defined(CXX_THREAD_AVAILABLE)
   void join_thread(std::thread* iThread){ 
      if(iThread->joinable()){iThread->join();}
   }
#endif
}

void WaitingTaskList_test::stressTest()
{
#if defined(CXX_THREAD_AVAILABLE)
   std::atomic<bool> called{false};
   std::atomic<int> count{0};
   demo::WaitingTaskList waitList;
   
   unsigned int index = 1000;
   const unsigned int nTasks = 10000;
   while(0 != --index) {
      called = false;
      count = 2;
      tbb::task_group group;
      {
	 std::thread makeTasksThread([&waitList,&count,&called,&group]{
            for(unsigned int i = 0; i<nTasks;++i) {
  	       auto t = new TestCalledTask{called,count};
               waitList.add(demo::WaitingTaskHolder(group,t));
            }
	    --count;
            });
         std::shared_ptr<std::thread> guard(&makeTasksThread,join_thread);
         
         std::thread doneWaitThread([&waitList,&called,&count]{
            called=true;
            waitList.doneWaiting(std::exception_ptr{});
	    --count;
            });
         std::shared_ptr<std::thread> guard2(&doneWaitThread,join_thread);
      }
      do {
	group.wait();
      }while(0 != count.load());
   }
#endif
}


CPPUNIT_TEST_SUITE_REGISTRATION( WaitingTaskList_test );
