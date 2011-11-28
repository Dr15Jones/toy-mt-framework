//
//  Filter.h
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef SingleThreadedProcessingDemo_Filter_h
#define SingleThreadedProcessingDemo_Filter_h

#include "Module.h"

namespace demo {
  class Event;
  namespace edm {
    class Event;
  }
  class Filter : public Module {
  public:
    Filter(const std::string& iLabel):
    Module(iLabel),
    m_keep(true) {
    }
    virtual ~Filter() {
    }
    bool doFilter(const Event& iEvent);    
  private:
    virtual bool filter(const edm::Event&) = 0;
    bool m_keep;
  };
}


#endif
