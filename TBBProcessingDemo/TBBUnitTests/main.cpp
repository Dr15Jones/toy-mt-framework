//
//  main.cpp
//  UnitTests
//
//  Created by Chris Jones on 9/27/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "Queues.h"

int main (int argc, const char * argv[])
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  
  demo::s_thread_safe_queue = dispatch_get_global_queue(0, 0);
  //demo::s_non_thread_safe_queue_stack.push_back(dispatch_queue_create("gov.fnal.non_thread_safe_0", NULL));
  //demo::s_non_thread_safe_queue_stack_back = demo::s_non_thread_safe_queue_stack.back();
  demo::s_non_thread_safe_queue = dispatch_queue_create("gov.fnal.non_thread_safe", NULL);
  dispatch_retain(demo::s_non_thread_safe_queue);

  
  bool wasSuccessful = runner.run( "", false );
  return wasSuccessful;
}

