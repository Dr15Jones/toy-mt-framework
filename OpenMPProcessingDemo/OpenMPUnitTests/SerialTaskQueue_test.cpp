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
#include "WaitingTask.h"
#include <memory>
#include "SerialTaskQueue.h"

class SerialTaskQueue_test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SerialTaskQueue_test);
  CPPUNIT_TEST(testPush);
  //CPPUNIT_TEST(testPushAndWait);
  CPPUNIT_TEST(testPause);
  CPPUNIT_TEST(stressTest);
  CPPUNIT_TEST_SUITE_END();
  
public:
  void testPush();
  //void testPushAndWait();
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
#pragma omp parallel default(shared)
      {
#pragma omp single
        {
          queue.push([&count]{
              CPPUNIT_ASSERT(count++ == 0);
              usleep(10);
            });
        
          queue.push([&count]{
              CPPUNIT_ASSERT(count++ == 1);
              usleep(10);
            });
          
          queue.push([&count]{
              CPPUNIT_ASSERT(count++ == 2);
              usleep(10);
            });
        }
      }

      CPPUNIT_ASSERT(count==3);
   }
}

/*
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
*/

void SerialTaskQueue_test::testPause()
{
   std::atomic<unsigned int> count{0};
   
   demo::SerialTaskQueue queue;   
   {
      queue.pause();
      {
        std::atomic<bool> allDone{false};
#pragma omp parallel default(shared)
        {
#pragma omp single
         {
           queue.push([&count,&allDone]{
               CPPUNIT_ASSERT(count++ == 0);
               allDone = true;
             });
           usleep(100);
           CPPUNIT_ASSERT(0==count);
           queue.resume();
         }
          //Need a task to stay around to keep the parallel block open
#pragma omp task 
         {
           while(not allDone.load() ) {
#pragma omp taskyield
           }
         }
        }
        CPPUNIT_ASSERT(count==1);
      }

      {
         std::atomic<unsigned int> allDone{3};
#pragma omp parallel default(shared)
         {
#pragma omp single
           {
             queue.push([&count,&queue,&allDone]{
                 queue.pause();
                 CPPUNIT_ASSERT(count++ == 1);
                 --allDone;
               });
             queue.push([&count,&queue,&allDone]{
                 CPPUNIT_ASSERT(count++ == 2);
                 --allDone;
               });
             queue.push([&count,&queue,&allDone]{
                 CPPUNIT_ASSERT(count++ == 3);
                 --allDone;
               });
             usleep(100);
             CPPUNIT_ASSERT(2==count);
             queue.resume();
#pragma omp task 
             {
               while(0 != allDone.load()) {
#pragma omp taskyield
               }
             }
           }
         }
         CPPUNIT_ASSERT(count==4);
      }
   }
   
}

void SerialTaskQueue_test::stressTest()
{
   demo::SerialTaskQueue queue;
   
   unsigned int index = 100;
   const unsigned int nTasks = 1000;
   while(0 != --index) {
     std::atomic<unsigned int> waitCount{1};
      std::atomic<unsigned int> count{0};
      
      std::atomic<bool> waitToStart{true};
#pragma omp parallel default(shared)
      {
#pragma omp single
        {
          ++waitCount;
          auto pushTask = demo::make_waiting_task([&queue,&waitToStart,&waitCount,&count](std::exception_ptr const*){
	    //gcc 4.7 doesn't preserve the 'atomic' nature of waitToStart in the loop
	    while(waitToStart.load()) {__sync_synchronize();};
            for(unsigned int i = 0; i<nTasks;++i) {
               ++waitCount;
               queue.push([i,&count,&waitCount] {
                  ++count;
                  --waitCount;
               });
            }
            --waitCount;
            });
         
         waitToStart=false;
         for(unsigned int i=0; i<nTasks;++i) {
            ++waitCount;
            queue.push([i,&count,&waitCount] {
               ++count;
               --waitCount;
             });
         }
         --waitCount;
#pragma omp task
         {
           while(waitCount != 0) {
#pragma omp taskyield
           }
         }
        }
      }
      CPPUNIT_ASSERT(2*nTasks==count);
   }
}


CPPUNIT_TEST_SUITE_REGISTRATION( SerialTaskQueue_test );
