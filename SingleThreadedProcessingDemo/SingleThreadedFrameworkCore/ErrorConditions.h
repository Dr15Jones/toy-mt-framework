//
//  ErrorConditions.h
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef SingleThreadedProcessingDemo_ErrorConditions_h
#define SingleThreadedProcessingDemo_ErrorConditions_h

namespace demo {
  enum ErrorConditions {
    kNoError=0,
    kStopProcessingEventError,
    kStopJobError
  };
}


#endif
