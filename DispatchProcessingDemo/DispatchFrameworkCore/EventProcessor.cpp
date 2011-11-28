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
  m_source(0),
  m_fatalJobErrorOccured(false)
  {
    m_schedules.push_back(new Schedule());
    m_schedules[0]->setFatalJobErrorOccurredPointer(&m_fatalJobErrorOccured);
    
  }
void 
EventProcessor::setSource(Source* iSource) {
  m_source = iSource;
}
void
EventProcessor::addPath(const std::string& iName,
                        const std::vector<std::string>& iModules) {
  m_schedules[0]->addPath(iModules);
}
void 
EventProcessor::addProducer(Producer* iProd) {
  m_schedules[0]->event()->addProducer(iProd);
}

void
EventProcessor::addFilter(Filter* iFilter) {
  m_schedules[0]->addFilter(iFilter);
}

static void get_and_process_one_event(
                               dispatch_group_t iEventGroup,
                               Source* iSource,
                               Schedule& iSchedule) {
  iSchedule.event()->reset();
  //iEvent.reset();
  
  if(iSource->setEventInfo(*(iSchedule.event()))) {
    //Make sure to wait for all work done by iSchedule
    dispatch_group_enter(iEventGroup);
    
    //items in a block become 'const' but we need non-const access to the event, hence use the pointer
    Schedule* nonConstSchedule = &iSchedule;
    iSchedule.process(^(bool iShouldContinue){
      if(iShouldContinue) {
        dispatch_group_async(iEventGroup, 
                             dispatch_get_global_queue(0, 0),
                             ^{get_and_process_one_event(iEventGroup,iSource,*nonConstSchedule);});
      };
      dispatch_group_leave(iEventGroup);
    });
  }
}

void EventProcessor::processAll(unsigned int iNumConcurrentEvents) {
  dispatch_group_t event_loop_group = dispatch_group_create();
  Schedule* scheduleTemp = m_schedules[0];
  dispatch_group_async(event_loop_group, 
                       dispatch_get_global_queue(0, 0), 
                       ^{
                         get_and_process_one_event(event_loop_group, m_source, *scheduleTemp);                           
                       });
  for(unsigned int nEvents = 1; nEvents<iNumConcurrentEvents; ++ nEvents) {
    Schedule* scheduleTemp = m_schedules[0]->clone();
    m_schedules.push_back(scheduleTemp);
    dispatch_group_async(event_loop_group, 
                         dispatch_get_global_queue(0, 0), 
                         ^{
                           get_and_process_one_event(event_loop_group, m_source, *scheduleTemp);                           
                         });
    
  }
  dispatch_group_wait(event_loop_group, DISPATCH_TIME_FOREVER);
  dispatch_release(event_loop_group);
}
