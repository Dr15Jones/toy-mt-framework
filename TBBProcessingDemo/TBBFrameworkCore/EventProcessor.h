//
//  EventProcessor.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_EventProcessor_h
#define DispatchProcessingDemo_EventProcessor_h
#include "Schedule.h"
#include "WaitingTaskHolder.h"
#include <vector>
#include <map>
#include <memory>
#include <exception>

namespace demo {
  class Source;
  class Schedule;
  class Event;
  class Path;
  class Producer;
  class Filter;
  class GetAndProcessOneEventTask;
  
  class EventProcessor {
     friend class GetAndProcessOneEventTask;
  public:
    EventProcessor();
    ~EventProcessor();
    
    void setSource(Source* iSource);
    void addPath(const std::string& iName, const std::vector<std::string>& iModules);
    void addProducer(Producer* iProd);
    void addFilter(Filter* iFilter);
    void processAll(unsigned int iNumConcurrentEvents);
    
    void finishSetup();
    
  private:

    void handleNextEventAsync(WaitingTaskHolder iHolder,
                              unsigned int iEventIndex);

    std::unique_ptr<Source> m_source;
    std::vector<std::unique_ptr<Schedule>> m_schedules;
    std::map<std::string,Producer*> m_producers;
    std::vector<Filter*> m_filters;
    unsigned int m_nextModuleID;

    std::atomic<bool> m_fatalJobErrorOccured;    
  };

}


#endif
