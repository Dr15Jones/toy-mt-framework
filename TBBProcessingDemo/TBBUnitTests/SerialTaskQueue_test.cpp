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
#include <memory>
#include "SerialTaskQueue.h"

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
#define CXX_THREAD_AVAILABLE
#endif

class SerialTaskQueue_test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SerialTaskQueue_test);
  CPPUNIT_TEST(testPush);
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
      tbb::task_group group;
      std::atomic<int> countdown{3};
   
      queue.push(group, [&count,&countdown]{
         CPPUNIT_ASSERT(count++ == 0);
         usleep(10);
	 --countdown;
      });

      queue.push(group, [&count,&countdown]{
         CPPUNIT_ASSERT(count++ == 1);
         usleep(10);
	 --countdown;
      });

      queue.push(group, [&count,&countdown]{
         CPPUNIT_ASSERT(count++ == 2);
         usleep(10);
	 --countdown;
      });

      do {
	group.wait();
      } while(0 != countdown.load());
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
  	 tbb::task_group group;
	 std::atomic<int> countdown(1);
   
         queue.push(group, [&count,&countdown]{
            CPPUNIT_ASSERT(count++ == 0);
	    --countdown;
         });
         usleep(100);
         CPPUNIT_ASSERT(0==count);
         queue.resume();

	 do {
	   group.wait();
	 }while( 0 != countdown.load());

         CPPUNIT_ASSERT(count==1);
      }

      {
  	 tbb::task_group group;
	 std::atomic<int> countdown(3);
         queue.push(group, [&count,&queue,&countdown]{
            queue.pause();
            CPPUNIT_ASSERT(count++ == 1);
	    --countdown;
         });
         queue.push(group, [&count,&queue,&countdown]{
            CPPUNIT_ASSERT(count++ == 2);
	    --countdown;
         });
         queue.push(group, [&count,&queue,&countdown]{
            CPPUNIT_ASSERT(count++ == 3);
	    --countdown;
         });
         usleep(100);
         CPPUNIT_ASSERT(2==count);
         queue.resume();
	 do {
	   group.wait();
	 }while(0!=countdown.load());

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
      std::atomic<unsigned int> count{0};

      tbb::task_group group;
      std::atomic<int> countdown{2};
      
      std::atomic<bool> waitToStart{true};
      {
    	 std::thread pushThread([&queue,&waitToStart,&group,&countdown,&count]{
	    //gcc 4.7 doesn't preserve the 'atomic' nature of waitToStart in the loop
	    while(waitToStart.load()) {__sync_synchronize();};
            for(unsigned int i = 0; i<nTasks;++i) {
 	       ++countdown;
               queue.push(group,[i,&count,&countdown] {
                  ++count;
		  --countdown;
               });
            }
         
	    --countdown;
	   });
         
         waitToStart=false;
         for(unsigned int i=0; i<nTasks;++i) {
  	    ++countdown;
            queue.push(group,[i,&count,&countdown] {
               ++count;
	       --countdown;
	    });
	 }
         --countdown;
         std::shared_ptr<std::thread>(&pushThread,join_thread);
      }
      do{
	group.wait();
      }while(0 != countdown);

      CPPUNIT_ASSERT(2*nTasks==count);
   }
#endif
}


CPPUNIT_TEST_SUITE_REGISTRATION( SerialTaskQueue_test );
