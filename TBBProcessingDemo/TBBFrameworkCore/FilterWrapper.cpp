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

FilterWrapper::FilterWrapper(std::shared_ptr<Filter> iFilter,Event* iEvent):
ModuleWrapper(iFilter.get(),iEvent),
m_filter(iFilter),
m_keep(false)
{
}

FilterWrapper::FilterWrapper(const FilterWrapper& iWrapper,Event* iEvent):
ModuleWrapper(iWrapper,iEvent),
m_filter(iWrapper.m_filter),
m_keep(false)
{
}

void
FilterWrapper::doFilterAsync(WaitingTask* iTask) {
  doWorkAsync(iTask);
}

void
FilterWrapper::reset()
{
  m_keep=false;
  ModuleWrapper::reset();
}

void
FilterWrapper::implDoWork()
{
  m_keep = filter()->doFilter(*event());
}

void
FilterWrapper::implDoAcquire(WaitingTaskWithArenaHolder h) {
  filter()->doAcquire(*event(), std::move(h));
}

const std::string&
FilterWrapper::label() const
{
  return filter()->label();
}
