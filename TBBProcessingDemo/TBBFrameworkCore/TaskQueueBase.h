/*
 *  TaskQueueBase.h
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/18/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef TBBProcessingDemo_TaskQueueBase_h
#define TBBProcessingDemo_TaskQueueBase_h

#include "tbb/task.h"
namespace demo {
   class TaskQueueBase {
   protected:
      class TaskBase : public tbb::task {
         friend class TaskQueueBase;
         TaskBase(): m_queue(0) {}

      protected:
         tbb::task* finishedTask();
      private:
         void setQueue(TaskQueueBase* iQueue) { m_queue = iQueue;}

         TaskQueueBase* m_queue;
      };
   
      template< typename T>
      class QueuedTask : public TaskBase {
      public:
        QueuedTask( const T& iAction):
        m_action(iAction) {}

      private:
        tbb::task* execute();

        T m_action;
      };
      
   public:
        TaskQueueBase()
        {  }

        virtual ~TaskQueueBase() {}

        template<typename T>
        void push(const T& iAction) {
           QueuedTask<T>* pTask{ new (tbb::task::allocate_root()) QueuedTask<T>{iAction} };
           pTask->setQueue(this);           
           pushTask(pTask);
        }

      private:
        virtual void pushTask(TaskBase*)=0;
        friend class TaskBase;
        virtual tbb::task* finishedTask() = 0;
   };

   inline
   tbb::task* 
   TaskQueueBase::TaskBase::finishedTask() {return m_queue->finishedTask();}
   
   template <typename T>
   tbb::task* 
   TaskQueueBase::QueuedTask<T>::execute() {
     try {
       m_action();
     } catch(...) {}
     return this->finishedTask();
   }
}

#endif