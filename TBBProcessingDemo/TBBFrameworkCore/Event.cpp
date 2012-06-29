/*
 *  Event.cpp
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#include <assert.h>
#include <iostream>
#include <cstdio>

#include "Event.h"
#include "Producer.h"
#include "ProducerWrapper.h"
#include "Queues.h"
#include "Getter.h"

using namespace demo;
typedef std::map<std::pair<std::string,std::string>, DataCache > LookupMap;

Event::Event():
m_lookupMap(),
m_index(0),
m_relativeSpeed(1)
{}

Event::Event(const Event& iOther):
m_index(iOther.m_index),
m_relativeSpeed(iOther.m_relativeSpeed)
{
  //NOTE: Items in the DataCache have references to the Event
  // so we need our own copy of them
  for(LookupMap::const_iterator it = iOther.m_lookupMap.begin(),itEnd=iOther.m_lookupMap.end();
      it != itEnd;
      ++it) {
    m_lookupMap.insert(std::make_pair(it->first, DataCache(it->second, this)));
  }  
}

int 
Event::get(const std::string& iModule, 
           const std::string& iProduct) const
{
   LookupMap::const_iterator it = m_lookupMap.find(std::make_pair(iModule,iProduct));
   assert(it != m_lookupMap.end());
   if(!it->second.wasCached()) {
      auto& waitList = it->second.producer()->doProduceAsync();
      tbb::empty_task* doneTask = new (tbb::task::allocate_root()) tbb::empty_task();
      waitList.add(doneTask);
      doneTask->wait_for_all();
      tbb::task::destroy(*doneTask);
   }
   return it->second.value();
}

int
Event::get(const Getter* iGetter) const
{
  //NOTE: If you have a Getter then we've already prefetched so 
  // this can be done synchronously
  
  LookupMap::const_iterator it = m_lookupMap.find(std::make_pair(iGetter->label(),iGetter->product()));

  assert(it != m_lookupMap.end());
  auto& gotten = it->second;
  bool wasCached = gotten.wasCached();
  if(not wasCached) {
    //assert(gotten.producer()->wasRun());
    std::cout << "Event::get called for missing item "<<iGetter->label()<<" "<<iGetter->product()<<" event #"<<index()<<std::endl;
  }
  assert(wasCached);
  return gotten.value();
}


//asynchronously get data
void
Event::getAsyncImpl(Getter* iGetter, tbb::task* iTaskDoneWithGet) const 
{
   //PROBLEM??: Is map find thread safe?  It looks like if I call find via two different threads
   // that I sometimes get the answer for the first thread when I'm running the second thread
   // this causes the wrong producer to be called
   //printf("   Event::getAsyncImp look for %s\n",iGetter->label().c_str());
   LookupMap::const_iterator it = m_lookupMap.find(std::make_pair(iGetter->label(),iGetter->product()));
   //printf("    found: %s %p\n",it->first.first.c_str(),it->second.first);
   if(it==m_lookupMap.end()) {
     printf("   Event::getAsyncImp failed to find %s '%s'\n",iGetter->label().c_str(),iGetter->product().c_str());
     assert(it != m_lookupMap.end());
     exit(1);
   }
   const DataCache* found = &(it->second);
   if(!found->wasCached()) {
      //printf("     getGroup: %s\n",found->first->label().c_str());
      auto& waitList = found->producer()->doProduceAsync();
      //Need to announce when we're done getting the data by leaving the group
      waitList.add(iTaskDoneWithGet);
   } else {
      //printf("     iGetter->set from already run producer %s %i\n",found->first->label().c_str(),found->second);
      //iGetter->set(found->value());
   }
}

void 
Event::getAsync(Getter* iGetter, tbb::task* iTaskDoneWithGet) const
{
  getAsyncImpl(iGetter, iTaskDoneWithGet);
}

void 
Event::put(const Producer* iProd, const std::string& iProduct, int iPut) {
   LookupMap::iterator itFind = m_lookupMap.find(std::make_pair(iProd->label(),iProduct));
   assert(itFind != m_lookupMap.end());
  itFind->second.setValue(iPut);
}


void 
Event::addProducer(Producer* iProd) {
   const std::set<DataKey>& keys = iProd->products();
   for(std::set<DataKey>::const_iterator it = keys.begin(), itEnd = keys.end();
       it != itEnd;
       ++it) {
     m_lookupMap.insert(std::make_pair(std::make_pair(iProd->label(), *it), DataCache(iProd,this)));
   }
}
void 
Event::reset()
{
   for(LookupMap::iterator it = m_lookupMap.begin(),itEnd=m_lookupMap.end();
       it != itEnd;
       ++it) {
      it->second.reset();
      it->second.producer()->reset();
   }
}

Event* 
Event::clone() {
  return new Event(*this);
}

namespace demo {
   namespace edm {
      //synchronously get data
      int Event::get(const std::string& iModule, 
                     const std::string& iProduct) const {
         if(!m_isThreadSafe) {
            //the calling module halted the non-thread safe queue
            // and we must restart it in case this get requires
            // running another non-thread-safe module
            s_non_thread_safe_queue->resume();
            //dispatch_resume(s_non_thread_safe_queue);
         }
         int returnValue = m_event->get(iModule,iProduct);
         if(!m_isThreadSafe) {
            //we must acquire the 'lock' on the non-thread-safe queue again
            s_non_thread_safe_queue->push([]{s_non_thread_safe_queue->pause();});
         }
         return returnValue;
      }
      
      int Event::get(const Getter* iGetter) const {
        return m_event->get(iGetter);
      }
     
      //asynchronously get data. The group will be incremented and will not be 
      // decremented until the attempt to get the data is finished
      void Event::getAsync(Getter* iGetter, tbb::task* iTask) const {
         m_event->getAsync(iGetter, iTask);
      }      
   }
}
