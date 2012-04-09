//
//  FilterWrapper.h
//  OpenMPProcessingDemo
//
//  Holds the per event information for a Filter
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef OpenMPProcessingDemo_FilterWrapper_h
#define OpenMPProcessingDemo_FilterWrapper_h
#if defined(PARALLEL_MODULES)  
#include <atomic>
#endif
#include <boost/shared_ptr.hpp>
#include "ModuleWrapper.h"
#include "Filter.h"

namespace demo {
  class Event;
  class FilterWrapper: public ModuleWrapper {
  public:
    FilterWrapper(boost::shared_ptr<Filter> iFilter);
    FilterWrapper(const FilterWrapper* iWrapper);
    
    void reset();
    const std::string& label() const;
    
    bool doFilter(Event&);
    
    bool wasRun() const { return m_wasRun;}
    boost::shared_ptr<Filter> filter() const;
    
  private:
    FilterWrapper(const FilterWrapper&) = delete;
    boost::shared_ptr<Filter> m_filter;
    bool m_keep;
#if defined(PARALLEL_MODULES)  
    std::atomic<bool> m_wasRun;
#else
    bool m_wasRun;
#endif    
  };
}

#endif
