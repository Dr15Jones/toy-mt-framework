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
#include "Path.h"
#include "Event.h"

namespace demo {
  class Path;
  class Event;
  class Filter;
  class FilterWrapper;
  class WaitingTaskHolder;
  
  class Schedule {
  public:
    
    Schedule();
    Schedule(const Schedule&);
    
    void processAsync(WaitingTaskHolder);

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

    //used for cloning
    Schedule(Event*);
    void addPath(Path* iPath);
    Event m_event;
    std::vector<std::shared_ptr<Path>> m_paths;
    std::vector<std::shared_ptr<FilterWrapper> > m_filters;
    std::atomic<bool>* m_fatalJobErrorOccuredPtr;
  };
}


#endif
