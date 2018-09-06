//
//  Path.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_Path_h
#define DispatchProcessingDemo_Path_h
#include <vector>
#include <memory>
#include <exception>
#include <atomic>
#include "FilterOnPathWrapper.h"
#include "WaitingTaskList.h"

namespace demo {
  class Event;
  class Filter;
  
  class Path {
  public:
    Path(): m_fatalJobErrorOccurredPtr(nullptr),m_callback() {}
    
    void runAsync(WaitingTask* iCallback); 
    
    void reset();
    
    void setFatalJobErrorOccurredPointer(std::atomic<bool>* iPtr) {
      m_fatalJobErrorOccurredPtr = iPtr;
    }
    
    void addFilter(FilterWrapper* iFilter, Event*);
    
    Path* clone(const std::vector<std::shared_ptr<FilterWrapper> >& iWrappers, Event*) const;

  private:    
    void filterFinished(std::exception_ptr const* iException,
                        size_t iIndex);


    Path(const Path& iOther) = delete;

    void runFilterAsync( size_t iIndex);
    
    std::vector<FilterOnPathWrapper> m_filters;
    std::atomic<bool>* m_fatalJobErrorOccurredPtr;
    WaitingTaskList m_callback;
  };
  
}

#endif
