//
//  ProducerWrapper.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <cassert>



#include "ProducerWrapper.h"
#include "Producer.h"
#include "Event.h"
#include "Queues.h"

using namespace demo;

static const std::string kPrefix("gov.fnal.");

static unsigned long nextID() {
  static unsigned long s_id =0;
  ++s_id;
  return s_id;
}

static std::string unique_name(const std::string& iString) {
  std::ostringstream os;
  os<<iString<<nextID();
  return os.str();
}

inline
Producer* 
ProducerWrapper::producer() const
{
  return static_cast<Producer*>(module());
}

ProducerWrapper::ProducerWrapper(Producer* iProd, Event* iEvent):
ModuleWrapper(iProd,iEvent),
PrefetchAndWorkWrapper(this),
m_producer(iProd),
m_waitingList{},
m_wasRun(false)
{
}

ProducerWrapper::ProducerWrapper(const ProducerWrapper& iOther,
                                 Event* iEvent):
ModuleWrapper(iOther,iEvent),
PrefetchAndWorkWrapper(this),
m_waitingList{},
m_producer(iOther.m_producer),
m_wasRun(false)
{
}

ProducerWrapper::ProducerWrapper(const ProducerWrapper& iOther):
ModuleWrapper(iOther),
PrefetchAndWorkWrapper(this),
m_waitingList{}, //we only use the copy constructor during cloning and we have no waiting at that time
m_producer(iOther.m_producer),
m_wasRun(iOther.m_wasRun)
{
}

ProducerWrapper::~ProducerWrapper()
{
}

void
ProducerWrapper::reset()
{
  m_wasRun=false;
  m_waitingList.reset();
  ModuleWrapper::reset();
}

WaitingList&
ProducerWrapper::doProduceAsync()
{
  if(m_wasRun) {
    return m_waitingList;
  }
  
  //NOTE: review this. I think we could just call 'doProduceAsyncImpl
  // directly since 'doPrefetchAndWork' does all of its activities asynchronously
  // anyway 
  
  //need to call group_async so that we 'enter' the group before returning
  // our value
  auto pThis = this;
  s_thread_safe_queue->push([pThis]{pThis->doProduceAsyncImpl();});
  return m_waitingList;
}


void
ProducerWrapper::doProduceAsyncImpl()
{
  if(not this->m_wasRun) {
    doPrefetchAndWork();
  }
}

void
ProducerWrapper::doWork()
{
  if (not this->m_wasRun) {
    //dispatch_debug(m_runQueue, "doProduceAsyncImpl %s %lu",producer()->label().c_str(),nonConstEvent->index());
    //NOTE: in real application this would have a 'try..catch' around it
    producer()->doProduce(*event());
    //NOTE: needs a memory barrier to guarantee that
    // m_wasRun is never set until after doFilter is run
    __sync_synchronize();
    this->m_wasRun = true;
  }
  
  m_waitingList.doneWaiting();
}
