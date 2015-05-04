//
//  SerialTaskQueue_test.cpp
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
#include "tbb/task.h"
#include <memory>
#include "SerialTaskQueue.h"

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
#define CXX_THREAD_AVAILABLE
#endif

class SerialTaskQueue_test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SerialTaskQueue_test);
  CPPUNIT_TEST(testPush);
  CPPUNIT_TEST(testPushAndWait);
  CPPUNIT_TEST(testPause);
  CPPUNIT_TEST(stressTest);
  CPPUNIT_TEST_SUITE_END();
  
public:
  void testPush();
  void testPushAndWait();
  void testPause();
  void stressTest();
  void setUp(){}
  void tearDown(){}
};

void SerialTaskQueue_test::testPush()
{
   std::atomic<unsigned int> count{0};
   
   demo::SerialTaskQueue queue;   
   {
      std::shared_ptr<tbb::task> waitTask{new (tbb::task::allocate_root()) tbb::empty_task{},
                                           [](tbb::task* iTask){tbb::task::destroy(*iTask);} };
      waitTask->set_ref_count(1+3);
      tbb::task* pWaitTask = waitTask.get();
   
      queue.push([&count,pWaitTask]{
         CPPUNIT_ASSERT(count++ == 0);
         usleep(10);
         pWaitTask->decrement_ref_count();
      });

      queue.push([&count,pWaitTask]{
         CPPUNIT_ASSERT(count++ == 1);
         usleep(10);
         pWaitTask->decrement_ref_count();
      });

      queue.push([&count,pWaitTask]{
         CPPUNIT_ASSERT(count++ == 2);
         usleep(10);
         pWaitTask->decrement_ref_count();
      });

      waitTask->wait_for_all();
      CPPUNIT_ASSERT(count==3);
   }
   
}

void SerialTaskQueue_test::testPushAndWait()
{
   std::atomic<unsigned int> count{0};
   
   demo::SerialTaskQueue queue;   
   {
      queue.push([&count]{
         CPPUNIT_ASSERT(count++ == 0);
         usleep(10);
      });

      queue.push([&count]{
         CPPUNIT_ASSERT(count++ == 1);
         usleep(10);
      });

      queue.pushAndWait([&count]{
         CPPUNIT_ASSERT(count++ == 2);
         usleep(10);
      }); 

      CPPUNIT_ASSERT(count==3);
   }
}

void SerialTaskQueue_test::testPause()
{
   std::atomic<unsigned int> count{0};
   
   demo::SerialTaskQueue queue;   
   {
      queue.pause();
      {
         std::shared_ptr<tbb::task> waitTask{new (tbb::task::allocate_root()) tbb::empty_task{},
                                             [](tbb::task* iTask){tbb::task::destroy(*iTask);} };
         waitTask->set_ref_count(1+1);
         tbb::task* pWaitTask = waitTask.get();
   
         queue.push([&count,pWaitTask]{
            CPPUNIT_ASSERT(count++ == 0);
            pWaitTask->decrement_ref_count();
         });
         usleep(100);
         CPPUNIT_ASSERT(0==count);
         queue.resume();
         waitTask->wait_for_all();
         CPPUNIT_ASSERT(count==1);
      }

      {
         std::shared_ptr<tbb::task> waitTask{new (tbb::task::allocate_root()) tbb::empty_task{},
                                             [](tbb::task* iTask){tbb::task::destroy(*iTask);} };
         waitTask->set_ref_count(1+3);
         tbb::task* pWaitTask = waitTask.get();
   
         queue.push([&count,&queue,pWaitTask]{
            queue.pause();
            CPPUNIT_ASSERT(count++ == 1);
            pWaitTask->decrement_ref_count();
         });
         queue.push([&count,&queue,pWaitTask]{
            CPPUNIT_ASSERT(count++ == 2);
            pWaitTask->decrement_ref_count();
         });
         queue.push([&count,&queue,pWaitTask]{
            CPPUNIT_ASSERT(count++ == 3);
            pWaitTask->decrement_ref_count();
         });
         usleep(100);
         CPPUNIT_ASSERT(2==count);
         queue.resume();
         waitTask->wait_for_all();
         CPPUNIT_ASSERT(count==4);
      }
   }
   
}

namespace {
#if defined(CXX_THREAD_AVAILABLE)
   void join_thread(std::thread* iThread){ 
      if(iThread->joinable()){iThread->join();}
   }
#endif
}

void SerialTaskQueue_test::stressTest()
{
#if defined(CXX_THREAD_AVAILABLE)
   demo::SerialTaskQueue queue;
   
   unsigned int index = 100;
   const unsigned int nTasks = 1000;
   while(0 != --index) {
      std::shared_ptr<tbb::task> waitTask{new (tbb::task::allocate_root()) tbb::empty_task{},
                                            [](tbb::task* iTask){tbb::task::destroy(*iTask);} };
      waitTask->set_ref_count(3);
      tbb::task* pWaitTask=waitTask.get();
      std::atomic<unsigned int> count{0};
      
      std::atomic<bool> waitToStart{true};
      {
         std::thread pushThread([&queue,&waitToStart,pWaitTask,&count]{
	    //gcc 4.7 doesn't preserve the 'atomic' nature of waitToStart in the loop
	    while(waitToStart.load()) {__sync_synchronize();};
            for(unsigned int i = 0; i<nTasks;++i) {
               pWaitTask->increment_ref_count();
               queue.push([i,&count,pWaitTask] {
                  ++count;
                  pWaitTask->decrement_ref_count();
               });
            }
         
            pWaitTask->decrement_ref_count();
            });
         
         waitToStart=false;
         for(unsigned int i=0; i<nTasks;++i) {
            pWaitTask->increment_ref_count();
            queue.push([i,&count,pWaitTask] {
               ++count;
               pWaitTask->decrement_ref_count();
            });
         }
         pWaitTask->decrement_ref_count();
         std::shared_ptr<std::thread>(&pushThread,join_thread);
      }
      waitTask->wait_for_all();

      CPPUNIT_ASSERT(2*nTasks==count);
   }
#endif
}


CPPUNIT_TEST_SUITE_REGISTRATION( SerialTaskQueue_test );
