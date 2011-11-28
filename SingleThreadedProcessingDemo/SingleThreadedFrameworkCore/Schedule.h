//
//  Schedule.h
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef SingleThreadedProcessingDemo_Schedule_h
#define SingleThreadedProcessingDemo_Schedule_h
#include <vector>
#include <string>

namespace demo {
  class Path;
  class Event;
  class Filter;
  
  class Schedule {
  public:
    Schedule()
    : 
    m_fatalJobErrorOccuredPtr(0){
    }
    void process(Event& iEvent);
    void reset();
    
    void setFatalJobErrorOccurredPointer(bool* iPtr) {
      m_fatalJobErrorOccuredPtr = iPtr;
    }
    
    Filter* findFilter(const std::string&);

    void addPath(const std::vector<std::string>& iPath);
    void addFilter(Filter*);
    
  private:
    void addPath(Path* iPath);
    
    std::vector<Path*> m_paths;
    std::vector<Filter*> m_filters;
    bool* m_fatalJobErrorOccuredPtr;
  };
}


#endif
