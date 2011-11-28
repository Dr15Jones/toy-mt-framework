//
//  Path.h
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef SingleThreadedProcessingDemo_Path_h
#define SingleThreadedProcessingDemo_Path_h
#include <vector>

namespace demo {
  class Event;
  class Filter;
  class Path {
  public:
    Path(): m_fatalJobErrorOccurredPtr(0) {}
    
    void run(Event& iEvent);    
    void reset();    
    void setFatalJobErrorOccurredPointer(bool* iPtr);
    void addFilter(Filter* iFilter);    
  private:
    std::vector<Filter*> m_filters;
    bool* m_fatalJobErrorOccurredPtr;
    
  };
}


#endif
