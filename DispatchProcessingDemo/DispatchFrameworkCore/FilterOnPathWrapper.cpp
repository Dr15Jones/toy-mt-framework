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

using namespace demo;

inline
FilterWrapper*
FilterOnPathWrapper::filter() const
{
  return m_filter;
}

FilterOnPathWrapper::FilterOnPathWrapper(FilterWrapper* iFilter, Path* iPath, size_t iIndex):
m_filter(iFilter),m_path(iPath),m_index(iIndex)
{
}


void
FilterOnPathWrapper::reset()
{
  m_filter->reset();
}

void
FilterOnPathWrapper::filterAsync(void(^iCallback)(bool,bool))
{
  m_filter->filterAsync(iCallback);
#if defined(NOT_DEFINED)
  if(not m_wasRun) {
    typedef void(^DoubleBoolCallback_t)(bool,bool);
    DoubleBoolCallback_t heapCallback = Block_copy(iCallback);
    
    /*NOTE: The m_runQueue is only used to serialize the doFilter
     and not for the getters. The reason is if the getters are 
     set multiple times it doesn't matter since they will be set
     with the exact same values. It only matters that the getters
     are set before the first time 'doFilter' is called, which
     is guaranteed by doPrefetchAndWork.
     If we do not want the getters to be called multiple times then
     we can do what we do for the EDProducers which is to use the
     module's queue to serialize both the gets and the 'do'.
     */
    doPrefetchAndWork(m_runQueue,
                      ^{
                        if(!m_wasRun) {
                          m_keep = filter()->doFilter(*(this->event()));
                        }
                        heapCallback(m_keep,true);
                        Block_release(heapCallback);
                      });
  } else {
    iCallback(m_keep,true);
  }
#endif
}


const std::string&
FilterOnPathWrapper::label() const
{
  return filter()->label();
}