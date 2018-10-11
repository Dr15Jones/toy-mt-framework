/*
 *  TaskBase.h
 *  OpenMPProcessingDemo
 *
 *  Created by Chris Jones on 9/18/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef OpenMPProcessingDemo_TaskBase_h
#define OpenMPProcessingDemo_TaskBase_h

#include <memory>

namespace demo {
  class TaskBase {
  public:
    TaskBase() = default;
    virtual ~TaskBase() = default;
    
    static void spawn(std::unique_ptr<TaskBase> iTask) {
      spawn(iTask.release());
    }
    
    //Takes ownership of iTask
    static void spawn(TaskBase* iTask) {
#pragma omp task untied firstprivate(iTask)
      {
        try {
          iTask->doSpawn();
        } catch(...) {}
        delete iTask;
      }
    }

  private:
    virtual void doSpawn() =0;    
  };
}

#endif
