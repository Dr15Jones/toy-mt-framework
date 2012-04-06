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
#include <boost/shared_ptr.hpp>
#include "Path.h"
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
    
    //returns true if should keep processing
    bool process();

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
    void processPresentPath(Path*);

    //used for cloning
    Schedule(Event*);
    void addPath(Path* iPath);
    Event m_event;
    std::vector<boost::shared_ptr<Path>> m_paths;
    std::vector<boost::shared_ptr<FilterWrapper> > m_filters;
    bool* m_fatalJobErrorOccuredPtr;
  };
}


#endif
