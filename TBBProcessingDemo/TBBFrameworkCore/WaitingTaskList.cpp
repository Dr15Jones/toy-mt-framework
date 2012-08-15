/*
 *  WaitingTaskList.cpp
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#include "WaitingTaskList.h"
#include "WaitableTask.h"

using namespace demo;


void
WaitingTaskList::add(WaitableTask* iTask) {
   iTask->increment_ref_count();
   if(!m_waiting) {
      if(0==iTask->decrement_ref_count()) {
         tbb::task::spawn(*iTask);
      }
   } else {
      WaitableTask* oldHead = m_head.exchange(iTask);
      if(oldHead) {
        iTask->setNextTask(oldHead);
        //NOTE: even if 'm_waiting' changed, we don't
        // have to recheck since we beat 'announce()' in
        // the ordering of 'm_head.exchange' call so iTask
        // is guaranteed to be in the link list
      } else {
        iTask->setNextTask(0);
        if(!m_waiting) {
           //if finished waiting right before we did the
           // exchange our task will not be spawned. Also,
           // additional threads may be calling add() and swapping
           // heads and linking us to the new head.
           // It is safe to call announce from multiple threads
           announce();
        }
        
      }
   }
}

//NOTE: Taken from libdispatch shims/atomics.h
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2)
#define hardware_pause()      asm("")
#endif
#if defined(__x86_64__) || defined(__i386__)
#undef hardware_pause
#define hardware_pause() asm("pause")
#endif

void
WaitingTaskList::announce()
{
   //Need a temporary storage since one of these tasks could
   // cause the next event to start processing which would refill 
   // this waiting list after it has been reset
   WaitableTask* t = m_head.exchange(0);
   WaitableTask* next;
   while(t) {
      //it is possible that 'WaitingTaskList::add' is running in a different
      // thread and we have a new 'head' but the old head has not yet been
      // attached to the new head (we identify this since 'nextTask' will return itself).
      //  In that case we have to wait until the link has been established before going on.
      // We also have to get 'next' before spawning t since spawning t will
      // eventually cause the task to be deleted and we don't want a race 
      // condition between delete and calling 'nextTask()'.
      while(t == (next=t->nextTask())) {
         hardware_pause();
      }
      if(0==t->decrement_ref_count()){
         tbb::task::spawn(*t);
      }
      t=next;
   }
}

void
WaitingTaskList::doneWaiting()
{
   m_waiting=false;
   announce();
}
