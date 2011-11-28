/*
 *  Queues.cpp
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#import "Queues.h"
namespace demo {
   dispatch_queue_t s_thread_safe_queue;
   dispatch_queue_t s_non_thread_safe_queue;
}
