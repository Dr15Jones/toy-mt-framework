/*
 *  TaskBase.h
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/18/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef TBBProcessingDemo_TaskBase_h
#define TBBProcessingDemo_TaskBase_h

#include <atomic>

namespace demo {
  class TaskBase {
  public:
    TaskBase() = default;
    TaskBase(TaskBase const&) = delete;
    TaskBase& operator=(TaskBase const&) = delete;
    virtual ~TaskBase() {}

    virtual void execute() = 0;

    void increment_ref_count() { ++refCount_;}
    bool decrement_ref_count() { return 0 == --refCount_;}
  private:
    std::atomic<unsigned int> refCount_{0};
  };
}  

#endif
