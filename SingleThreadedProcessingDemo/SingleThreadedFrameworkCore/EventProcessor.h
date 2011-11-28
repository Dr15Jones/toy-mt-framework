//
//  EventProcessor.h
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef SingleThreadedProcessingDemo_EventProcessor_h
#define SingleThreadedProcessingDemo_EventProcessor_h
#include "Schedule.h"
#include "Event.h"

namespace demo {
  class Source;
  class Path;
  class Producer;
  
  class EventProcessor {
  public:
    EventProcessor();
    
    void setSource(Source* iSource) {
      m_source = iSource;
    }
    void addPath(const std::string& iName, const std::vector<std::string>& iModules);
    void addProducer(Producer* iProd) {
      m_event.addProducer(iProd);
    }
    void addFilter(Filter* iFilter);
    void processAll();
  private:
    Source* m_source;
    Schedule m_schedule;
    Event m_event;
    bool m_fatalJobErrorOccured;    
  };
  
}

#endif
