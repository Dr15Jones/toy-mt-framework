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
    friend class TaskSentry;

    TaskBase() = default;
    TaskBase(TaskBase const&) = delete;
    TaskBase& operator=(TaskBase const&) = delete;
    virtual ~TaskBase() {}

    virtual void execute() = 0;

    void increment_ref_count() { ++refCount_;}
    unsigned int decrement_ref_count() { return --refCount_;}
  private:
    virtual void recycle() { delete this;}
    std::atomic<unsigned int> refCount_{0};
  };

  class TaskSentry {
  public:
    TaskSentry(TaskBase* iTask): task_{iTask} {}
    ~TaskSentry() { task_->recycle();}
  private:
    TaskBase* task_;
  };
}  

#endif
