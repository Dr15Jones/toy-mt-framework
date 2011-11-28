//
//  Source.h
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef SingleThreadedProcessingDemo_Source_h
#define SingleThreadedProcessingDemo_Source_h

namespace demo {
  class Event;
  class Source {
  public:
    virtual ~Source();
    virtual bool setEventInfo(Event&) = 0;
  };
}

#endif
