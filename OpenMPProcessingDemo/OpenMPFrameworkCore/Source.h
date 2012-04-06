//
//  Source.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_Source_h
#define DispatchProcessingDemo_Source_h
namespace demo {
  class Event;
  
  class Source {
  public:
    virtual bool setEventInfo(Event&) = 0;
  };
}

#endif
