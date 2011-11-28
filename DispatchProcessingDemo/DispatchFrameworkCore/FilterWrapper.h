//
//  FilterWrapper.h
//  DispatchProcessingDemo
//
//  Holds the per event information for a Filter
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_FilterWrapper_h
#define DispatchProcessingDemo_FilterWrapper_h

#include <dispatch/dispatch.h>
#include "GroupHolder.h"
#include "ModuleWrapper.h"

namespace demo {
  class Filter;
  class Event;
  class FilterWrapper: private ModuleWrapper {
  public:
    explicit FilterWrapper(Filter* iFilter, Event* iEvent);
    FilterWrapper(const FilterWrapper&, Event*);
    FilterWrapper(const FilterWrapper&);
    ~FilterWrapper();
    
    void filterAsync(const Event&,
                     void(^iCallback)(bool,bool));
    void reset();
    const std::string& label() const;
    
  private:
    Filter* filter() const;
    dispatch_queue_t m_runQueue;
    bool m_keep;
    bool m_wasRun;    
  };
}

#endif
