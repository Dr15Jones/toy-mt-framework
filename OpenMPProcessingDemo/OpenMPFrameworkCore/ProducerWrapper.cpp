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

ProducerWrapper::ProducerWrapper(Producer* iProd):
ModuleWrapper(iProd),
m_producer(iProd),
m_wasRun(false)
{
}

ProducerWrapper::ProducerWrapper(const ProducerWrapper* iOther):
ModuleWrapper(iOther),
m_producer(iOther->m_producer),
m_wasRun(false)
{
}

ProducerWrapper::ProducerWrapper(const ProducerWrapper& iOther):
ModuleWrapper(iOther),
m_producer(iOther.m_producer),
m_wasRun(false)
{
}


ProducerWrapper::~ProducerWrapper()
{
}

void
ProducerWrapper::reset()
{
  m_wasRun=false;
}

void
ProducerWrapper::doProduce(Event& iEvent)
{
  if(m_wasRun) {
    return;
  }
  prefetch(iEvent);
  if(!m_wasRun) {
    OMPLockSentry(runLock());
    if(!m_wasRun) {
    m_producer->doProduce(iEvent);
    //NOTE: needs a memory barrier to guarantee that
    // m_wasRun is never set until after doFilter is run
    __sync_synchronize();
    this->m_wasRun = true;
    }
  }
}
