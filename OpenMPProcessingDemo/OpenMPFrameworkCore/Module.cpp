/*
 *  Module.cpp
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#include <iostream>

#include "Module.h"
#include "Getter.h"
#include "Event.h"

using namespace demo;

Module::~Module()
{}


void
Module::prefetch(const Event& iEvent)
{
  for (std::list<Getter>::iterator it=m_getters.begin(),itEnd=m_getters.end();
       it!=itEnd; ++it) {
    Getter* temp = &(*it);
#pragma omp task untied default(shared), firstprivate(temp)
    iEvent.prefetch(temp);
  }
  #pragma omp taskwait
}

const Getter* 
Module::registerGet(const std::string& iLabel, const std::string& iProduct)
{
   Getter g(iLabel,iProduct);
   m_getters.push_back(g);
   return &(m_getters.back());
}
