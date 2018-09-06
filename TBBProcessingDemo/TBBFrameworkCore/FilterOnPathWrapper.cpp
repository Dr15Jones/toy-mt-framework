//
//  FilterOnPathWrapper.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <sstream>

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
                                         size_t iIndex):
m_filter(iFilter),m_index(iIndex)
{
}

bool
FilterOnPathWrapper::checkResultsOfRunFilter() {
  return m_filter->shouldKeep();
}

void
FilterOnPathWrapper::filterAsync(WaitingTask* iTask)
{
  m_filter->doFilterAsync(iTask);
}


const std::string&
FilterOnPathWrapper::label() const
{
  return filter()->label();
}
