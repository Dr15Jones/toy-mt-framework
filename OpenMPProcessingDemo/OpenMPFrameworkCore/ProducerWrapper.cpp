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
#include <exception>


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


Producer* 
ProducerWrapper::producer() const
{
  return static_cast<Producer*>(module());
}

ProducerWrapper::ProducerWrapper(Producer* iProd, Event* iEvent):
ModuleWrapper(iProd,iEvent),
m_producer(iProd)
{
}

ProducerWrapper::ProducerWrapper(const ProducerWrapper& iOther,
                                 Event* iEvent):
ModuleWrapper(iOther,iEvent),
m_producer(iOther.m_producer)
{
}

ProducerWrapper::ProducerWrapper(const ProducerWrapper& iOther):
ModuleWrapper(iOther),
m_producer(iOther.m_producer)
{
}

void
ProducerWrapper::implDoWork() {
  producer()->doProduce(*event());
}

void
ProducerWrapper::doProduceAsync(WaitingTaskHolder iCallTaskWhenDone)
{
  doWorkAsync(std::move(iCallTaskWhenDone));
}
