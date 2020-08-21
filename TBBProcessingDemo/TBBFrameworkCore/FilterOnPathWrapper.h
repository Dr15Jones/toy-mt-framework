//
//  FilterOnPathWrapper.h
//  DispatchProcessingDemo
//
//  Holds the information about which and where on a Path a filter resides
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_FilterOnPathWrapper_h
#define DispatchProcessingDemo_FilterOnPathWrapper_h

namespace demo {
  class FilterWrapper;
  class Path;
  class WaitingTaskHolder;

  class FilterOnPathWrapper  {
  public:
    FilterOnPathWrapper(FilterWrapper* iFilter,
                        size_t iIndex);
    
    void filterAsync(WaitingTaskHolder);
    const std::string& label() const;

    FilterWrapper* filter() const;

    bool checkResultsOfRunFilter();

  private:
    FilterWrapper* m_filter;
    size_t m_index;
  };
}

#endif
