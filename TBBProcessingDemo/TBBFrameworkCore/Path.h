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
#include "PathFilteringCallback.h"
#include "FilterOnPathWrapper.h"


namespace demo {
  class Event;
  class Filter;
  
  class Path {
  public:
    Path(): m_fatalJobErrorOccurredPtr(0),m_callback(0) {}
    
    void runAsync(PathFilteringCallback iCallback); 
    
    void reset();
    
    void setFatalJobErrorOccurredPointer(std::atomic<bool>* iPtr) {
      m_fatalJobErrorOccurredPtr = iPtr;
    }
    
    void addFilter(FilterWrapper* iFilter, Event*);
    
    Path* clone(const std::vector<std::shared_ptr<FilterWrapper> >& iWrappers, Event*) const;
    
    void doNextIfSuccess(bool iKeep, std::exception_ptr iException, size_t iPreviousIndex);
  private:
    friend class FilterOnPathWrapper;
    Path(const Path& iOther); //undefined

    void runFilterAsync( size_t iIndex);
    
    std::vector<FilterOnPathWrapper> m_filters;
    std::atomic<bool>* m_fatalJobErrorOccurredPtr;
    PathFilteringCallback m_callback;
  };
  
}

#endif
