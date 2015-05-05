/*
 *  Queues.h
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
 #include <memory>
 #include "TaskQueueBase.h"
 #include "ConcurrentTaskQueue.h"
 #include "SerialTaskQueue.h"
//a stack of queues which are managing work which is not thread safe
// we need multiple queues because when one is blocked we need to start 
// the next one. When the next one is finished the queue goes away and
// the previous one on the stack is unblocked and starts running
//We store the 'back' of the stack separately in order to allow thread safe
// reading of the back in away that isn't dependent on how 'push_back' and 'back'
// would interact if called from different threads
namespace demo {
   extern std::shared_ptr<SerialTaskQueue> s_non_thread_safe_queue;
}
