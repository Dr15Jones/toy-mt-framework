/*
 *  ConcurrentTaskQueue.cpp
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#include "ConcurrentTaskQueue.h"

using namespace demo;

void
ConcurrentTaskQueue::pushTask(ConcurrentTaskQueue::TaskBase* iTask) {
  if(0!=iTask) {
    tbb::task::spawn(*iTask);      
  }
}

tbb::task*
ConcurrentTaskQueue::finishedTask() {
   return 0;
}
