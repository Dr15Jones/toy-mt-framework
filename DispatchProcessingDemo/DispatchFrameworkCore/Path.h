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
#include "filtering_callback.h"

namespace demo {
  class Event;
  class FilterWrapper;
  class Filter;
  
  class Path {
  public:
    Path(): m_fatalJobErrorOccurredPtr(0) {}
    
    void runAsync(Event& iEvent, filtering_callback_t iCallback); 
    
    void reset();
    
    void setFatalJobErrorOccurredPointer(bool* iPtr) {
      m_fatalJobErrorOccurredPtr = iPtr;
    }
    
    void addFilter(FilterWrapper* iFilter);
    
    Path* clone(const std::vector<FilterWrapper*>& iWrappers) const;
  private:
    Path(const Path& iOther);

    //NOTE: iCallback must be on the heap
    void runFilterAsync(Event& iEvent, 
                        unsigned int iIndex,
                        filtering_callback_t iCallback) const;
    
    std::vector<FilterWrapper*> m_filters;
    bool* m_fatalJobErrorOccurredPtr;
    
  };
  
}

#endif
