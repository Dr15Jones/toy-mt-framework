//
//  ModuleThreadStack.h
//  OpenMPProcessingDemo
//
//  Created by Chris Jones on 12/21/12.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef OpenMPProcessingDemo_DataCache_h
#define OpenMPProcessingDemo_DataCache_h

#include <vector>

namespace demo {
  
  class ModuleThreadStack {
  public:
    
    struct TransitionModuleID {
      unsigned int transitionID;
      unsigned int moduleID;
    };
    
    static void push(unsigned int iTransitionID, unsigned int iModuleID);
    static void pop();
  
    static std::vector<TransitionModuleID> const& stack() { return privateStack();}
  private:
    ModuleThreadStack() = delete;
    static std::vector<TransitionModuleID>& privateStack();
  };
  
  class ModuleThreadStackSentry {
  public:
    ModuleThreadStackSentry(unsigned int iTransitionID, unsigned int iModuleID) {
      ModuleThreadStack::push(iTransitionID, iModuleID);
    }
    ~ModuleThreadStackSentry() {
      ModuleThreadStack::pop();
    }
  };
}


#endif