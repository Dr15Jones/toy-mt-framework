//
//  EventProcessor.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_EventProcessor_h
#define DispatchProcessingDemo_EventProcessor_h
#include <vector>
#include <atomic>
#include "Schedule.h"
#include "OMPLock.h"
#include <boost/shared_ptr.hpp>

namespace demo {
  class Source;
  class Schedule;
  class Event;
  class Path;
  class Producer;
  class Filter;
  
  class EventProcessor {
  public:
    EventProcessor();
    ~EventProcessor();
    
    void setSource(Source* iSource);
    void addPath(const std::string& iName, const std::vector<std::string>& iModules);
    void addProducer(Producer* iProd);
    void addFilter(Filter* iFilter);
    void processAll(unsigned int iNumConcurrentEvents);
    
  private:
    void get_and_process_events(Schedule&);
    
    boost::shared_ptr<Source> m_source;
    OMPLock m_sourceLock;
    std::vector<boost::shared_ptr<Schedule>> m_schedules;
    std::atomic<bool> m_fatalJobErrorOccured;    
  };

}


#endif
