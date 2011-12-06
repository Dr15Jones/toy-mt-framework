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
#include "PrefetchAndWorkWrapper.h"

namespace demo {
  class FilterWrapper;
  class Path;
  class FilterOnPathWrapper: public PrefetchAndWorkWrapper {
  public:
    FilterOnPathWrapper(FilterWrapper* iFilter,
                        Path* iPath,
                        size_t iIndex);
    
    void filterAsync();
    void reset();
    const std::string& label() const;

    FilterWrapper* filter() const;

  private:
    void doWork();
    FilterWrapper* m_filter;
    Path* m_path;
    size_t m_index;
  };
}

#endif
