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
#include <atomic>

namespace demo {
  class Event;
  
  class SimpleSource : public Source {
  public:
    SimpleSource(size_t iNEvents);
    
    virtual bool setEventInfo(Event& iEvent);
  private:
    const size_t m_nEvents;
    std::atomic<size_t> m_seenEvents;
  };
}


#endif
