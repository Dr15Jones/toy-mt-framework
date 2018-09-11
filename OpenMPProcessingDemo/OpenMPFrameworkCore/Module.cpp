/*
 *  Module.cpp
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#include <iostream>
#include <algorithm>
#include <cassert>
#include "WaitingTaskHolder.h"
#include "Module.h"
#include "Getter.h"
#include "Event.h"

using namespace demo;

Module::~Module()
{}


void
Module::prefetchAsync(const Event& iEvent, WaitingTaskHolder iTask)
{
  for (std::list<Getter>::iterator it=m_getters.begin(),itEnd=m_getters.end();
       it!=itEnd; ++it) {
    iEvent.getAsync(&(*it),iTask);
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

void 
Module::setDependentModuleToCheck(std::vector<unsigned int>& iModuleIDs)
{
   m_dependentModulesToCheck.swap(iModuleIDs);
}

bool 
Module::isADependentModule(unsigned int iModuleID) const
{
   return std::binary_search(m_dependentModulesToCheck.begin(),m_dependentModulesToCheck.end(),iModuleID);
}

