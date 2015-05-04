//
//  ModuleThreadStack.cpp
//  TBBProcessingDemo
//
//  Created by Chris Jones on 12/21/12.
//  Copyright 2012 FNAL. All rights reserved.
//

#include "ModuleThreadStack.h"

namespace demo {
  void ModuleThreadStack::push(unsigned int iTransitionID, unsigned int iModuleID) {
    privateStack().push_back(TransitionModuleID{iTransitionID,iModuleID});
  }
  
  void ModuleThreadStack::pop() {
    privateStack().pop_back();
  }
  
  std::vector<ModuleThreadStack::TransitionModuleID>& ModuleThreadStack::privateStack() {
    thread_local std::vector<TransitionModuleID> s_stack;
    return s_stack;
  }
}
