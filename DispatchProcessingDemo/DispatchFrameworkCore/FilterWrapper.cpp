//
//  FilterWrapper.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <Block.h>
#include <cassert>

#include "FilterWrapper.h"
#include "Filter.h"

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
Filter*
FilterWrapper::filter() const
{
  return static_cast<Filter*>(module());
}

FilterWrapper::FilterWrapper(Filter* iFilter, Event* iEvent):
ModuleWrapper(iFilter,iEvent),
m_runQueue(dispatch_queue_create(unique_name(kPrefix+iFilter->label()).c_str(), NULL)),
m_keep(false),
m_wasRun(false)
{
  //dispatch_retain(m_runQueue);
}

FilterWrapper::FilterWrapper(const FilterWrapper& iOther, Event* iEvent):
ModuleWrapper(iOther,iEvent),
m_keep(false),
m_wasRun(false)
{
  if(filter()->threadType() == kThreadSafeBetweenInstances) {
    //the same instance can be called reentrantly so each Schedule can have
    // its own queue for each instance rather than having to share one queue
    m_runQueue = dispatch_queue_create(unique_name(kPrefix+filter()->label()).c_str(), NULL);
  } else {
    m_runQueue=iOther.m_runQueue;
    dispatch_retain(m_runQueue);
  }
}

FilterWrapper::FilterWrapper(const FilterWrapper& iOther):
ModuleWrapper(iOther),
m_runQueue(iOther.m_runQueue),
m_keep(iOther.m_keep),
m_wasRun(iOther.m_wasRun)
{
  dispatch_retain(m_runQueue);
}

FilterWrapper::~FilterWrapper()
{
  dispatch_release(m_runQueue);
}

void
FilterWrapper::reset()
{
  m_wasRun=false;
  m_keep=false;
  ModuleWrapper::reset();
}

void
FilterWrapper::filterAsync(const Event& iEvent,
                      void(^iCallback)(bool,bool))
{
  assert(&iEvent == event());
  if(not m_wasRun) {
    typedef void(^DoubleBoolCallback_t)(bool,bool);
    DoubleBoolCallback_t heapCallback = Block_copy(iCallback);
    
    /*NOTE: The m_runQueue is only used to serialize the doFilter
     and not for the getters. The reason is if the getters are 
     set multiple times it doesn't matter since they will be set
     with the exact same values. It only matters that the getters
     are set before the first time 'doFilter' is called, which
     is guaranteed by doPrefetchAndWork.
     If we do not want the getters to be called multiple times then
     we can do what we do for the EDProducers which is to use the
     module's queue to serialize both the gets and the 'do'.
     */
    doPrefetchAndWork(m_runQueue,
                      ^{
                        if(!m_wasRun) {
                          m_keep = filter()->doFilter(iEvent);
                        }
                        heapCallback(m_keep,true);
                        Block_release(heapCallback);
                      });
  } else {
    iCallback(m_keep,true);
  }
}

const std::string&
FilterWrapper::label() const
{
  return filter()->label();
}