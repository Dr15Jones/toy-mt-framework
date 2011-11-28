//
//  Filter.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/13/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_Filter_h
#define DispatchProcessingDemo_Filter_h
#import <dispatch/dispatch.h>

#include"Module.h"

namespace demo {
  class Event;
  namespace edm {
    class Event;
  }
  class Filter : public Module {
  public:
    Filter(const std::string& iLabel,
           ThreadType iThreadType=kThreadUnsafe);
    virtual ~Filter();
    
    bool doFilter(const Event& iEvent);
    
    Filter( const Filter& iOther);
    
  private:
    
    virtual bool filter(const edm::Event&) = 0;
  };
}

#endif
