//
//  FactoryManagers.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/29/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>

#include "FactoryManager.h"
#include "Producer.h"
#include "Filter.h"

INSTANTIATE_FACTORYMANAGER(demo::Producer);
INSTANTIATE_FACTORYMANAGER(demo::Filter);
