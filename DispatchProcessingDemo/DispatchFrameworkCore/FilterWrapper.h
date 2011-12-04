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


namespace demo {
  class Filter;
  class Event;
  class FilterWrapper{
  public:
    FilterWrapper(Filter* iFilter);
    
    void reset();
    const std::string& label() const;
    
    bool doFilter(Event& iEvent);
    
    bool wasRun() const { return m_wasRun;}
    Filter* filter() const;
    
  private:
    Filter* m_filter;
    bool m_keep;
    bool m_wasRun;    
  };
}

#endif
