//
//  thread_type_from_config.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_thread_type_from_config_h
#define DispatchProcessingDemo_thread_type_from_config_h

#include "Module.h"
#include <boost/property_tree/ptree.hpp>

demo::ThreadType thread_type_from_config(const boost::property_tree::ptree&);

#endif
