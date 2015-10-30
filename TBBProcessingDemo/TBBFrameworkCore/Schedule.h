//
//  Schedule.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_Schedule_h
#define DispatchProcessingDemo_Schedule_h
#include <vector>
#include <string>
#include <atomic>
#include <memory>
#include "PathFilteringCallback.h"
#include "Path.h"
#include "Event.h"

namespace demo {
  class Path;
  class Event;
  class Filter;
  class FilterWrapper;
  
  //NOTE: The implementation of ScheduleFilteringCallback is
  // in the EventProcessor since it is the only class which
  // ever requests a callback
  class ScheduleFilteringCallback {
  public:
    explicit ScheduleFilteringCallback(void* iContext=0):
    m_context(iContext) {}
    
    void operator()(std::exception_ptr) const;
  private:
    void* m_context;
  };
  
  
  class Schedule {
  public:
    friend class PathFilteringCallback;
    
    Schedule();
    Schedule(const Schedule&);
    
    void process(ScheduleFilteringCallback iCallback);

    void reset();
    
    void addPath(const std::vector<std::string>& iPath);
    void addFilter(Filter*);
    
    FilterWrapper* findFilter(const std::string&);
    
    void setFatalJobErrorOccurredPointer(std::atomic<bool>* iPtr) {
      m_fatalJobErrorOccuredPtr = iPtr;
    }
    
    Event* event();
    Schedule* clone();
    
  private:
    void processPresentPath(Path*);

    //used for cloning
    Schedule(Event*);
    void addPath(Path* iPath);
    void aPathHasFinished(std::exception_ptr iException);
    Event m_event;
    std::vector<std::shared_ptr<Path>> m_paths;
    std::vector<std::shared_ptr<FilterWrapper> > m_filters;
    std::atomic<unsigned int> m_pathsStillRunning;
    PathFilteringCallback m_pathDoneCallback;
    ScheduleFilteringCallback m_scheduleCallback;
    std::atomic<bool>* m_fatalJobErrorOccuredPtr;
  };
}


#endif
