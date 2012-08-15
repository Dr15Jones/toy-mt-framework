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
    
  bool wasSuccessful = runner.run( "", false );
  return wasSuccessful;
}

