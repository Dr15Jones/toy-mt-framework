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
#include <map>
#include "Schedule.h"
#include <boost/shared_ptr.hpp>
#include <tbb/task.h>

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
    
    class LoopContext {
       friend class EventProcessor;
    public: 
      LoopContext():m_schedule(),m_processor(0) {}
      void filter(bool);
      boost::shared_ptr<Schedule> schedule() const { return m_schedule;}
      EventProcessor* processor() const {return m_processor;}
    private:
      LoopContext(Schedule* iSchedule, EventProcessor* iProc):
      m_schedule(iSchedule), m_processor(iProc) {}
      boost::shared_ptr<Schedule> m_schedule;
      EventProcessor* m_processor;
    };
    friend class LoopContext;
  private:
    
    boost::shared_ptr<Source> m_source;
    tbb::task* m_eventLoopWaitTask;
    std::vector<LoopContext> m_schedules;
    std::map<std::string,Producer*> m_producers;
    std::vector<Filter*> m_filters;
    unsigned int m_nextModuleID;
    bool m_fatalJobErrorOccured;    
  };

}


#endif
