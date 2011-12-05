//
//  EventProcessor.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include "EventProcessor.h"
#include "Schedule.h"
#include "Event.h"
#include "Path.h"
#include "Source.h"

using namespace demo;

EventProcessor::EventProcessor():
  m_source(),
  m_eventLoopGroup(dispatch_group_create()),
  m_fatalJobErrorOccured(false)
  {
    m_schedules.push_back(LoopContext(new Schedule(),this));
    m_schedules[0].m_schedule->setFatalJobErrorOccurredPointer(&m_fatalJobErrorOccured);
    
  }

EventProcessor::~EventProcessor()
{
  dispatch_release(m_eventLoopGroup);
}

void 
EventProcessor::setSource(Source* iSource) {
  m_source = boost::shared_ptr<Source>(iSource);
}
void
EventProcessor::addPath(const std::string& iName,
                        const std::vector<std::string>& iModules) {
  m_schedules[0].m_schedule->addPath(iModules);
}
void 
EventProcessor::addProducer(Producer* iProd) {
  m_schedules[0].m_schedule->event()->addProducer(iProd);
}

void
EventProcessor::addFilter(Filter* iFilter) {
  m_schedules[0].m_schedule->addFilter(iFilter);
}

void
ScheduleFilteringCallback::operator()(bool iShouldContinue) const {
  EventProcessor::LoopContext* lc = static_cast<EventProcessor::LoopContext*>(m_context);
  lc->filter(iShouldContinue);
}

void
EventProcessor::LoopContext::filter(bool iShouldContinue) {
  dispatch_group_t eventGroup = m_processor->m_eventLoopGroup;

  if(iShouldContinue) {
    dispatch_group_async_f(eventGroup, 
                           dispatch_get_global_queue(0, 0),
                           this,
                           &EventProcessor::get_and_process_one_event_f);
  };
  dispatch_group_leave(eventGroup);
}

void 
EventProcessor::get_and_process_one_event_f(void * context) 
{
  EventProcessor::LoopContext* lc = static_cast<EventProcessor::LoopContext*>(context);
  dispatch_group_t eventGroup = lc->m_processor->m_eventLoopGroup;
  Source* source = lc->m_processor->m_source.get();
  Schedule& schedule = *(lc->m_schedule);
  schedule.event()->reset();
  
  if(source->setEventInfo(*(schedule.event()))) {
    //Make sure to wait for all work done by iSchedule
    dispatch_group_enter(eventGroup);
    
    schedule.process(ScheduleFilteringCallback(context));
  }
}

void EventProcessor::processAll(unsigned int iNumConcurrentEvents) {
  m_schedules.reserve(iNumConcurrentEvents);
  dispatch_group_async_f(m_eventLoopGroup, 
                         dispatch_get_global_queue(0, 0),
                         &m_schedules[0],
                         &EventProcessor::get_and_process_one_event_f);
  for(unsigned int nEvents = 1; nEvents<iNumConcurrentEvents; ++ nEvents) {
    Schedule* scheduleTemp = m_schedules[0].m_schedule->clone();
    m_schedules.push_back(LoopContext(scheduleTemp,this));
    dispatch_group_async_f(m_eventLoopGroup, 
                           dispatch_get_global_queue(0, 0),
                           &m_schedules.back(),
                           &EventProcessor::get_and_process_one_event_f);    
  }
  dispatch_group_wait(m_eventLoopGroup, DISPATCH_TIME_FOREVER);
}
