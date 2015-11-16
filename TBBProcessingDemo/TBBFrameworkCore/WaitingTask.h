/*
 *  WaitingTaskList.h
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/18/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef TBBProcessingDemo_WaitingTask_h
#define TBBProcessingDemo_WaitingTask_h

#include <atomic>
#include <exception>
#include "tbb/task.h"

namespace demo {
  class WaitingTask : public tbb::task {
   public:
    WaitingTask() : m_ptr{nullptr} {}
    ~WaitingTask() override {
       delete m_ptr.load();
    }

    void dependentTaskFailed(std::exception_ptr iPtr) {
       if(iPtr and not m_ptr) {
          //auto temp = std::make_unique<std::exception_ptr>(iPtr);
          std::unique_ptr<std::exception_ptr> temp{new std::exception_ptr{iPtr}};
          std::exception_ptr* expected = nullptr;
          if( m_ptr.compare_exchange_strong(expected, temp.get()) ) {
             temp.release();
          }
       }
    }
    
    std::exception_ptr * exceptionPtr() {
       return m_ptr.load();
    }
    
   private:
      std::atomic<std::exception_ptr*> m_ptr;
  };
  
  class EmptyWaitingTask : public WaitingTask {
  public:
     EmptyWaitingTask() = default;
     
     tbb::task* execute() override {}
  };
}

#endif