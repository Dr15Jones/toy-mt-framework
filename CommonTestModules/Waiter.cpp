//
//  Waiter.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include "Waiter.h"
#include "Event.h"

using namespace demo;

Waiter::Waiter(const boost::property_tree::ptree& iConfig)
{
  m_min = iConfig.get<double>("minTime",0.);
  double max = iConfig.get<double>("maxTime",0.);
  double average = iConfig.get<double>("averageTime");
  
  if(max == 0.) {
    //using set value 
    m_min = average;
    m_relativeSpeedSwitch=0.;
    m_t1 = 0.;
    m_t2 =0.;
  } else {
    m_t1 = average - m_min;
    m_t2 = max - average;
    m_relativeSpeedSwitch = m_t2/(m_t1+m_t2);
  }

}

void Waiter::wait(const edm::Event& iEvent)
{
  if(0. == m_relativeSpeedSwitch) {
    doWait(m_min);
  } else {
    double relativeSpeed=iEvent.relativeSpeed();
    double time=0.;
    if (relativeSpeed < m_relativeSpeedSwitch) {
      time = m_min+m_t1/m_relativeSpeedSwitch*relativeSpeed;
    } else {
      time = m_min + m_t1 +m_t2*relativeSpeed + m_t2/m_t1*(relativeSpeed -1.0);
    }
    doWait(time);
  }
}