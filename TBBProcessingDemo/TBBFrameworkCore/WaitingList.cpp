/*
 *  WaitingList.cpp
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#include "WaitingList.h"

using namespace demo;


void
WaitingList::add(tbb::task* iTask) {
   iTask->increment_ref_count();
   if(!m_waiting) {
      if(0==iTask->decrement_ref_count()) {
         tbb::task::spawn(*iTask);
      }
   } else {
      m_tasks.push(iTask);
      if(!m_waiting) {
         //if finished waiting before push then our
         // task may not have been spawned.
         // It is safe to call announce from multiple threads
         announce();
      }
   }
}

void
WaitingList::announce()
{
   tbb::task* t=0;
   while(m_tasks.try_pop(t)) {
      if(0==t->decrement_ref_count()) {
         tbb::task::spawn(*t);
      }
   }
}

void
WaitingList::doneWaiting()
{
   m_waiting=false;
   announce();
}
