//
//  FilterWrapper.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <Block.h>
#include <cassert>

#include "FilterWrapper.h"
#include "Filter.h"

using namespace demo;

inline
Filter*
FilterWrapper::filter() const
{
  return m_filter;
}

FilterWrapper::FilterWrapper(Filter* iFilter):
m_filter(iFilter),
m_keep(false),
m_wasRun(false)
{
}

void
FilterWrapper::reset()
{
  m_wasRun=false;
  m_keep=false;
}

bool
FilterWrapper::doFilter(Event& iEvent)
{
  if(!m_wasRun) {
    m_keep = filter()->doFilter(iEvent);
    m_wasRun=true;
  }
  return m_keep;
}

const std::string&
FilterWrapper::label() const
{
  return filter()->label();
}