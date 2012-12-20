/*
 *  Module.cpp
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#include <iostream>
#include "tbb/task.h"
#include "Module.h"
#include "Getter.h"
#include "Event.h"

using namespace demo;

Module::~Module()
{}


void
Module::prefetchAsync(const Event& iEvent, tbb::task* iTask)
{
  assert(0!=iTask);
  //need to be sure that ref count isn't set to 0 immediately
  iTask->increment_ref_count();
  for (std::list<Getter>::iterator it=m_getters.begin(),itEnd=m_getters.end();
       it!=itEnd; ++it) {
    iEvent.getAsync(&(*it),iTask);
  }
  if(0==iTask->decrement_ref_count()) {
     //if everything finishes before we leave this routine, we need to launch the task
     tbb::task::spawn(*iTask);
  }
}

const Getter* 
Module::registerGet(const std::string& iLabel, const std::string& iProduct)
{
   Getter g(iLabel,iProduct);
   m_getters.push_back(g);
   return &(m_getters.back());
}

void 
Module::registerMightGet(const std::string& iLabel, const std::string& iProduct)
{
  Getter g(iLabel,iProduct);
  m_mightGetters.push_back(g);
}
