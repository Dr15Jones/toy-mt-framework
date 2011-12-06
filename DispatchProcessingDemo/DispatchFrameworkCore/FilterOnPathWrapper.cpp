//
//  FilterOnPathWrapper.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <Block.h>
#include <cassert>

#include "FilterOnPathWrapper.h"
#include "FilterWrapper.h"
#include "Filter.h"
#include "Path.h"

using namespace demo;

inline
FilterWrapper*
FilterOnPathWrapper::filter() const
{
  return m_filter;
}

FilterOnPathWrapper::FilterOnPathWrapper(FilterWrapper* iFilter,
                                         Path* iPath,
                                         size_t iIndex):
demo::PrefetchAndWorkWrapper(iFilter),
m_filter(iFilter),m_path(iPath),m_index(iIndex)
{
}


void
FilterOnPathWrapper::reset()
{
  m_filter->reset();
}

void
FilterOnPathWrapper::doWork()
{
  bool keep = m_filter->doFilter();
  m_path->doNextIfSuccess(keep, true, m_index);
}

void
FilterOnPathWrapper::filterAsync()
{
  if(not m_filter->wasRun()) {    
    doPrefetchAndWork();
  } else {
    //Still need to cal 'doWork' since that is where the
    // path gets informed that the work was done
    this->doWork();
  }
}


const std::string&
FilterOnPathWrapper::label() const
{
  return filter()->label();
}