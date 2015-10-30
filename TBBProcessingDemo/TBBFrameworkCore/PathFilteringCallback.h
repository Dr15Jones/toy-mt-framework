//
//  PathFilteringCallback.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_PathFilteringCallback_h
#define DispatchProcessingDemo_PathFilteringCallback_h

#include <exception>

namespace demo {
  class Schedule;
  class EventProcessor;
  
  class PathFilteringCallback {
  public:
    explicit PathFilteringCallback(Schedule* iSchedule=0):
    m_schedule(iSchedule) {}
    
    void operator()(std::exception_ptr) const;
  private:
    Schedule* m_schedule;
  };  
}

#endif
