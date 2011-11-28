//
//  SimpleSource.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_SimpleSource_h
#define DispatchProcessingDemo_SimpleSource_h
#include "Source.h"

namespace demo {
  class Event;
  
  class SimpleSource : public Source {
  public:
    SimpleSource(unsigned int iNEvents);
    
    virtual bool setEventInfo(Event& iEvent);
  private:
    unsigned int m_nEvents;
    unsigned int m_seenEvents;
  };
}


#endif
