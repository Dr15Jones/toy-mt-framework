//
//  SimpleSource.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include "SimpleSource.h"
#include "Event.h"

using namespace demo;

SimpleSource::SimpleSource(unsigned int iNEvents): 
  m_nEvents(iNEvents),
  m_seenEvents(0) {}
  
bool 
SimpleSource::setEventInfo(Event& iEvent) {
  //printf("event #%u\n",static_cast<unsigned int>(m_seenEvents));
  iEvent.setIndex(m_seenEvents);
  return m_nEvents >= ++m_seenEvents;
}
