//
//  Waiter.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_Waiter_h
#define DispatchProcessingDemo_Waiter_h
#include <boost/property_tree/ptree.hpp>

namespace demo {
  namespace edm {
    class Event;
  }
  class Waiter {
  public:
    Waiter(const boost::property_tree::ptree& iConfig);
    
    void wait(const edm::Event&);
    
    virtual void doWait(double iTimeInSeconds) =0;
    
  private:
    double m_min;
    double m_t1;
    double m_t2;
    double m_relativeSpeedSwitch;
  };
}


#endif
