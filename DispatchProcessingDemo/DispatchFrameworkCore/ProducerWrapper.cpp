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

#include <Block.h>

#include "ProducerWrapper.h"
#import "Producer.h"
#import "Event.h"
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
m_runQueue(dispatch_queue_create(unique_name(kPrefix+iProd->label()).c_str(), NULL)),
m_group(dispatch_group_create()),
m_work(createWorkBlock()),
m_wasRun(false)
{
}

ProducerWrapper::ProducerWrapper(const ProducerWrapper& iOther,
                                 Event* iEvent):
ModuleWrapper(iOther,iEvent),
m_group(dispatch_group_create()),
m_work(createWorkBlock()),
m_wasRun(false)
{
  if(producer()->threadType() == kThreadSafeBetweenInstances) {
    //the same instance can be called reentrantly so each Schedule can have
    // its own queue for each instance rather than having to share one queue
    m_runQueue = dispatch_queue_create(unique_name(kPrefix+producer()->label()).c_str(), NULL);
  } else {
    m_runQueue=iOther.m_runQueue;
    dispatch_retain(m_runQueue);
  }
}

ProducerWrapper::ProducerWrapper(const ProducerWrapper& iOther):
ModuleWrapper(iOther),
m_runQueue(iOther.m_runQueue),
m_group(iOther.m_group),
m_work(createWorkBlock()),//NOTE: need to create new blocks since they internally hold the 'this' pointer
m_wasRun(iOther.m_wasRun)
{
  dispatch_retain(m_runQueue);
}

ProducerWrapper::~ProducerWrapper()
{
  dispatch_release(m_runQueue);
  Block_release(m_work);
}

void
ProducerWrapper::reset()
{
  m_wasRun=false;
  ModuleWrapper::reset();
}

GroupHolder
ProducerWrapper::doProduceAsync()
{
  if(m_wasRun) {
    return m_group;
  }
  
  //need to call group_async so that we 'enter' the group before returning
  // our value
  dispatch_group_async_f(m_group.get(),s_thread_safe_queue ,
                         static_cast<void*>(this),
                         ProducerWrapper::do_produceAsyncImpl_task);
  
  return m_group;
}


void
ProducerWrapper::doProduceAsyncImpl()
{
  if(not this->m_wasRun) {
    dispatch_group_enter(m_group.get());
    doPrefetchAndWork(m_runQueue, m_work);
  }
}

void
ProducerWrapper::do_produceAsyncImpl_task(void* iContext)
{
  ProducerWrapper* wrapper = reinterpret_cast<ProducerWrapper*>(iContext);
  wrapper->doProduceAsyncImpl();
}

void
ProducerWrapper::doWork()
{
  if (not this->m_wasRun) {
    //dispatch_debug(m_runQueue, "doProduceAsyncImpl %s %lu",producer()->label().c_str(),nonConstEvent->index());
    //NOTE: in real application this would have a 'try..catch' around it
    producer()->doProduce(*event());
    this->m_wasRun = true;
  }
  
  //dispatch_debug(m_getQueue, "doProduceAsyncImpl %s %lu",label().c_str(),iEvent.index());
  //dispatch_debug(produceDoneGroup.get(), "doProduceAsyncImpl %s %lu",producer()->label().c_str(),nonConstEvent->index());
  dispatch_group_leave(this->m_group.get());  
}

ProducerWrapper::WorkBlock_t
ProducerWrapper::createWorkBlock()
{
  return Block_copy(^{
    this->doWork();
  });
}