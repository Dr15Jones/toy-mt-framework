//
//  Module.cpp
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 8/5/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>

#include "Module.h"
#include "Getter.h"
#include "Event.h"

using namespace demo;

Module::~Module(){}

const Getter* 
Module::registerGet(const std::string& iLabel, const std::string& iProduct)
{
  Getter* g = new Getter(iLabel,iProduct);
  m_getters.push_back(g);
  return g;
}

void 
Module::prefetch(const Event& iEvent)
{
  if(hasPrefetchItems()) {
    for (std::vector<Getter*>::iterator it=m_getters.begin(),itEnd=m_getters.end();
         it!=itEnd; ++it) {
      iEvent.get(*it);
    }
  }
}