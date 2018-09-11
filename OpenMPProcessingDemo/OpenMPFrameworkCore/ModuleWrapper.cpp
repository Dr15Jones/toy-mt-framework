//
//  ModuleWrapper.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/8/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <cassert>

#include "ModuleWrapper.h"
#include "WaitingTaskHolder.h"
#include "Module.h"
#include "Queues.h"
#include "Event.h"
#include "SerialTaskQueue.h"

using namespace demo;

ModuleWrapper::ModuleWrapper(Module* iModule, Event* iEvent):
m_module(iModule),
m_event(iEvent),
m_runQueue( m_module->threadType() == kThreadSafeBetweenInstances? static_cast<SerialTaskQueue*>(nullptr) : new SerialTaskQueue{}),
m_workStarted{false}
{
}

ModuleWrapper::ModuleWrapper(const ModuleWrapper& iOther,
                             Event* iEvent):
m_module(iOther.m_module),
m_event(iEvent),
m_workStarted{false}
{
  if(m_module->threadType() != kThreadSafeBetweenInstances) {
    //the same instance can not be called reentrantly so each Schedule must
    // have its own queue for each instance
    m_runQueue=iOther.m_runQueue;
  }
}

ModuleWrapper::ModuleWrapper(const ModuleWrapper& iOther):
m_module(iOther.m_module),
m_event(iOther.m_event),
m_runQueue(iOther.m_runQueue),
m_workStarted{false}
{
}

ModuleWrapper&
ModuleWrapper::operator=(const ModuleWrapper& iOther)
{
  if(&iOther!=this) {
    m_module =iOther.m_module;
    m_event = iOther.m_event;
    m_runQueue=iOther.m_runQueue;
    //leave it the way it was
    //m_workStarted = iOther.m_workStarted
  }
  return *this;
}

ModuleWrapper::~ModuleWrapper()
{
}

void
ModuleWrapper::doWorkAsync(WaitingTaskHolder iTask) {
  m_waitingTasks.add(std::move(iTask));

  bool expected = false;
  if(m_workStarted.compare_exchange_strong(expected,true) ) {
    module()->prefetchAsync(*m_event, 
                            WaitingTaskHolder(make_waiting_task([this](std::exception_ptr const* iPtr)
      {
        std::exception_ptr ptr;
        if(iPtr) {
          ptr = *iPtr;
        }
        if(runQueue()) {
          runQueue()->push([this,ptr]() {
              runModuleAfterAsyncPrefetch(ptr);
            });
        } else {
          runModuleAfterAsyncPrefetch(ptr);
        }
      }) )
    );
  }
}

void 
ModuleWrapper::prefetchAsync(WaitingTaskHolder iPrefetchDoneTask)
{
  if(module()->hasPrefetchItems()) { 
     module()->prefetchAsync(*m_event, std::move(iPrefetchDoneTask)); 
  }
}

void
ModuleWrapper::runModuleAfterAsyncPrefetch(std::exception_ptr iPtr) {
  if(not iPtr) {
    try {
      implDoWork();
    } catch(...) {
      iPtr = std::current_exception();
    }
  }
  m_waitingTasks.doneWaiting(iPtr);
};
