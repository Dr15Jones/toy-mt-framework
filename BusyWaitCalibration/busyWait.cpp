//
//  busyWait.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <cmath>
#include "busyWait.h"

namespace demo {
float busyWait(unsigned long long iCount) {
  float value = 0;
  for (int i=0; i<iCount; ++i) {
    value += sin(float(i)/100);
  }
  return std::abs(value);
}
}