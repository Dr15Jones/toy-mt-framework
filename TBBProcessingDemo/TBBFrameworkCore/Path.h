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
#include <boost/shared_ptr.hpp>
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
    
    void setFatalJobErrorOccurredPointer(bool* iPtr) {
      m_fatalJobErrorOccurredPtr = iPtr;
    }
    
    void addFilter(FilterWrapper* iFilter, Event*);
    
    Path* clone(const std::vector<boost::shared_ptr<FilterWrapper> >& iWrappers, Event*) const;
    
    void doNextIfSuccess(bool iKeep, bool iSuccess, size_t iPreviousIndex);
  private:
    friend class FilterOnPathWrapper;
    Path(const Path& iOther); //undefined

    static void reset_f(void*, size_t);
    
    //NOTE: iCallback must be on the heap
    void runFilterAsync( size_t iIndex);
    
    std::vector<FilterOnPathWrapper> m_filters;
    bool* m_fatalJobErrorOccurredPtr;
    PathFilteringCallback m_callback;
  };
  
}

#endif
