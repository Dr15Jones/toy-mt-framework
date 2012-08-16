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
#include <atomic>


#include "ProducerWrapper.h"
#include "Producer.h"
#include "Event.h"
#include "Queues.h"

//std::atomic<unsigned long> s_numberOfTasks{0};

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

void
ProducerWrapper::doProduceAsync(tbb::task* iCallTaskWhenDone)
{
  //if it has already run or we are not the first task
  // to request the Producer to be run then just return the
  // wait list without scheduling the Producer to be run.
  //This is needed so we don't have redundant tasks (i.e. ones where an earlier
  // task did the same work) from previous events hanging around when the next
  // event starts. This can happen since we don't wait for ALL tasks to complete
  // we only wait for the results for a task to be available.
  //std::cout <<"     requested to run "<<m_producer->label()<<std::endl;
  //if(m_wasRun or m_requestedToRun.test_and_set()) {
  m_waitingList.add(iCallTaskWhenDone);
  if(m_wasRun) {
    //std::cout <<"     "<<m_producer->label() <<"already run"<<std::endl;
    return;
  }
  
  //NOTE: review this. I think we could just call 'doProduceAsyncImpl
  // directly since 'doPrefetchAndWork' does all of its activities asynchronously
  // anyway 

  //SECOND NOTE: Since we now only request to run on the first request I can
  // get rid of the m_requestedPrefetch in the base class since we will only
  // ask to prefetch once now
  
  //++s_numberOfTasks;
  auto pThis = this;
  s_thread_safe_queue->push([pThis]{pThis->doProduceAsyncImpl();});
  return;
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
    //std::cout <<" ProducerWrapper::doWork finished "<<producer()->label()<<std::endl;
    m_waitingList.doneWaiting();
  }
}
