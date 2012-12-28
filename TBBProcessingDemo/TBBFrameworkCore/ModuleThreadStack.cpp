//
//  ModuleThreadStack.cpp
//  TBBProcessingDemo
//
//  Created by Chris Jones on 12/21/12.
//  Copyright 2012 FNAL. All rights reserved.
//

#include <boost/thread/tss.hpp>
#include "ModuleThreadStack.h"

namespace demo {
  void ModuleThreadStack::push(unsigned int iTransitionID, unsigned int iModuleID) {
    privateStack().push_back(TransitionModuleID{iTransitionID,iModuleID});
  }
  
  void ModuleThreadStack::pop() {
    privateStack().pop_back();
  }
  
  std::vector<ModuleThreadStack::TransitionModuleID>& ModuleThreadStack::privateStack() {
    static boost::thread_specific_ptr<std::vector<TransitionModuleID>> pStack;
    if(0==pStack.get()) {
      pStack.reset(new std::vector<TransitionModuleID>{});
      pStack->reserve(2);
    }
    return *(pStack.get());
  }
}