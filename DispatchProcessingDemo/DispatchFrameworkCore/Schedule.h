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
#include "GroupHolder.h"
#include "filtering_callback.h"
#include "Event.h"

namespace demo {
  class Path;
  class Event;
  class Filter;
  class FilterWrapper;
  
  class Schedule {
  public:
    Schedule();
    Schedule(const Schedule&);
    
    void process(filtering_callback_t iCallback);

    void reset();
    
    void addPath(const std::vector<std::string>& iPath);
    void addFilter(Filter*);
    
    FilterWrapper* findFilter(const std::string&);
    
    void setFatalJobErrorOccurredPointer(bool* iPtr) {
      m_fatalJobErrorOccuredPtr = iPtr;
    }
    
    Event* event();
    Schedule* clone();
  private:
    //NOTE: Must pass a heap based callback to this code
    void processPresentPath(unsigned int iIndex);

    //used for cloning
    Schedule(Event*);
    void addPath(Path* iPath);
    Event m_event;
    std::vector<Path*> m_paths;
    std::vector<FilterWrapper*> m_filters;
    GroupHolder m_allPathsDoneGroup;
    bool* m_fatalJobErrorOccuredPtr;
  };
}


#endif
