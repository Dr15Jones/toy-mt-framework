//
//  EventProcessor.cpp
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include "EventProcessor.h"
#include "Source.h"

using namespace demo;

EventProcessor::EventProcessor():
m_source(0),
m_fatalJobErrorOccured(false)
{
  m_schedule.setFatalJobErrorOccurredPointer(&m_fatalJobErrorOccured);
}

void
EventProcessor::addPath(const std::string& iName,
                        const std::vector<std::string>& iModules) {
  m_schedule.addPath(iModules);
}

void
EventProcessor::addFilter(Filter* iFilter) {
  m_schedule.addFilter(iFilter);
}

void 
EventProcessor::processAll() {
  while(m_source->setEventInfo(m_event)) {
    m_schedule.process(m_event);
    m_event.reset();
  }
}

