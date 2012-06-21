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
  m_eventLoopWaitTask(new (tbb::task::allocate_root()) tbb::empty_task{}),
  m_fatalJobErrorOccured(false)
  {
    m_eventLoopWaitTask->increment_ref_count();
    m_schedules.push_back(LoopContext(new Schedule(),this));
    m_schedules[0].m_schedule->setFatalJobErrorOccurredPointer(&m_fatalJobErrorOccured);
    
  }

EventProcessor::~EventProcessor()
{
  tbb::task::destroy(*m_eventLoopWaitTask);
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

namespace demo {
   class GetAndProcessOneEventTask : public tbb::task {
      public:
         GetAndProcessOneEventTask(EventProcessor::LoopContext& iContext):
         m_context(iContext){}
      
         tbb::task* execute() {
            Source* source = m_context.processor()->m_source.get();
            Schedule& schedule = *(m_context.schedule());
            schedule.event()->reset();

            if(source->setEventInfo(*(schedule.event()))) {
              schedule.process(ScheduleFilteringCallback(&m_context));
            } else {
               //no more work to do so signal that this schedule has finished
               m_context.processor()->m_eventLoopWaitTask->decrement_ref_count();
            }
            return 0;
         }
      private:
         EventProcessor::LoopContext& m_context;
   };
} 

void
EventProcessor::LoopContext::filter(bool iShouldContinue) {
  if(iShouldContinue) {    
    tbb::task::spawn( *(new (tbb::task::allocate_root()) GetAndProcessOneEventTask{*this}));
  } else {
     //told to stop processing so we signal that this schedule has finished
     m_processor->m_eventLoopWaitTask->decrement_ref_count();
  }
}
/*
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
*/
void EventProcessor::processAll(unsigned int iNumConcurrentEvents) {
  m_schedules.reserve(iNumConcurrentEvents);

  for(unsigned int nEvents = 1; nEvents<iNumConcurrentEvents; ++ nEvents) {
    Schedule* scheduleTemp = m_schedules[0].m_schedule->clone();
    m_schedules.push_back(LoopContext(scheduleTemp,this));
    m_eventLoopWaitTask->increment_ref_count();
    tbb::task::spawn( *(new (tbb::task::allocate_root()) GetAndProcessOneEventTask{m_schedules.back()}));
  }
  //Do this after all others so that we are not calling 'Event->clone()' while the
  // object is being accessed on another thread
  m_eventLoopWaitTask->increment_ref_count();
  tbb::task::spawn( *(new (tbb::task::allocate_root()) GetAndProcessOneEventTask{m_schedules[0]}));
  
  m_eventLoopWaitTask->wait_for_all();
}
