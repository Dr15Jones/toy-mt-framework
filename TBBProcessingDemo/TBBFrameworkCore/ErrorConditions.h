//
//  ErrorConditions.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ErrorConditions_h
#define DispatchProcessingDemo_ErrorConditions_h

namespace demo {
  enum ErrorConditions {
    kNoError=0,
    kStopProcessingEventError,
    kStopJobError
  };
}

#endif
