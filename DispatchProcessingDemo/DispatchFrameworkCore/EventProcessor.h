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
#include "Schedule.h"
#include <boost/shared_ptr.hpp>
#include <dispatch/dispatch.h>

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
    
    class LoopContext {
      friend class EventProcessor;
    public: 
      LoopContext():m_schedule(),m_processor(0) {}
      void filter(bool);
    private:
      LoopContext(Schedule* iSchedule, EventProcessor* iProc):
      m_schedule(iSchedule), m_processor(iProc) {}
      boost::shared_ptr<Schedule> m_schedule;
      EventProcessor* m_processor;
    };
    friend class LoopContext;
  private:
    static void get_and_process_one_event_f(void*);
    
    boost::shared_ptr<Source> m_source;
    dispatch_group_t m_eventLoopGroup;
    std::vector<LoopContext> m_schedules;
    bool m_fatalJobErrorOccured;    
  };

}


#endif
