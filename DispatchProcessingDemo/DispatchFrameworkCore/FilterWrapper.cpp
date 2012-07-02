//
//  FilterWrapper.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <sstream>

#include <cassert>

#include "FilterWrapper.h"
#include "Filter.h"

using namespace demo;

inline
Filter*
FilterWrapper::filter() const
{
  return m_filter.get();
}

FilterWrapper::FilterWrapper(boost::shared_ptr<Filter> iFilter,Event* iEvent):
ModuleWrapper(iFilter.get(),iEvent),
m_filter(iFilter),
m_keep(false),
m_wasRun(false)
{
}

FilterWrapper::FilterWrapper(const FilterWrapper& iWrapper,Event* iEvent):
ModuleWrapper(iWrapper,iEvent),
m_filter(iWrapper.m_filter),
m_keep(false),
m_wasRun(false)
{
}


void
FilterWrapper::reset()
{
  m_wasRun=false;
  m_keep=false;
  ModuleWrapper::reset();
}

bool
FilterWrapper::doFilter()
{
  if(!m_wasRun) {
    m_keep = filter()->doFilter(*event());
    //NOTE: needs a memory barrier to guarantee that
    // m_wasRun is never set until after doFilter is run
    __sync_synchronize();
    m_wasRun=true;
  }
  return m_keep;
}

const std::string&
FilterWrapper::label() const
{
  return filter()->label();
}
