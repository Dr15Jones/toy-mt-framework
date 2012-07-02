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
#include <atomic>
#include <boost/shared_ptr.hpp>
#include "ModuleWrapper.h"
#include "Filter.h"

namespace demo {
  class Event;
  class FilterWrapper: public ModuleWrapper {
  public:
    FilterWrapper(boost::shared_ptr<Filter> iFilter,Event*);
    FilterWrapper(const FilterWrapper& iWrapper, Event* iEvent);
    
    void reset();
    const std::string& label() const;
    
    bool doFilter();
    
    bool wasRun() const { return m_wasRun;}
    Filter* filter() const;
    
  private:
    FilterWrapper(const FilterWrapper&);
    boost::shared_ptr<Filter> m_filter;
    bool m_keep;
    std::atomic<bool> m_wasRun;    
  };
}

#endif
