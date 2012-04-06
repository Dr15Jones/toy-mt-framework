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
  m_fatalJobErrorOccured(false)
  {
    m_schedules.push_back( boost::shared_ptr<Schedule>(new Schedule()));
    m_schedules[0]->setFatalJobErrorOccurredPointer(&m_fatalJobErrorOccured);
  }

EventProcessor::~EventProcessor()
{
}

void 
EventProcessor::setSource(Source* iSource) {
  m_source = boost::shared_ptr<Source>(iSource);
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

void 
EventProcessor::get_and_process_events(Schedule& iSchedule) 
{
  bool shouldContinue = true;
  do {
    Source* source = m_source.get();
    iSchedule.event()->reset();

    {
      OMPLockSentry sentry(&m_sourceLock);
      shouldContinue = source->setEventInfo(*(iSchedule.event()));
    }
    if(shouldContinue) {
      shouldContinue=iSchedule.process();
    }

  } while(shouldContinue && not m_fatalJobErrorOccured);
}

void EventProcessor::processAll(unsigned int iNumConcurrentEvents) {
  m_schedules.reserve(iNumConcurrentEvents);
#pragma omp parallel default(shared)
  {
    #pragma omp single
    {
      for(unsigned int nEvents = 1; nEvents<iNumConcurrentEvents; ++ nEvents) {
	boost::shared_ptr<Schedule> scheduleTemp{m_schedules[0]->clone()};
	m_schedules.push_back(scheduleTemp);
	#pragma omp task untied
	{
	  get_and_process_events(*(m_schedules.back()));
	}
      }
      //Do this after all others so that we are not calling 'Event->clone()' while the
      // object is being accessed on another thread
      get_and_process_events(*(m_schedules[0]));
    }
  }
}
