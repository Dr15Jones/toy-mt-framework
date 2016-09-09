//
//  main.cpp
//  BusyWaitCalibration
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include "calibrate.h"


int main (int argc, const char * argv[])
{

  calibrate(100000);
  calibrate(1000000);
  calibrate(10000000);
  calibrate(100000000);
  return 0;
}

