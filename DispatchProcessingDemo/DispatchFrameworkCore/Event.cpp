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
#include <cstring>

#include "Event.h"
#include "Producer.h"
#include "ProducerWrapper.h"
#include "Queues.h"
#include "Getter.h"

using namespace demo;
typedef std::map<Event::LabelAndProduct, DataCache > LookupMap;

inline
bool
Event::LabelAndProduct::operator<(const Event::LabelAndProduct& iRHS) const {
  int comp = std::strcmp(m_label, iRHS.m_label);
  if(comp==0) {
    comp = std::strcmp(m_product,iRHS.m_product);
  }
  return comp<0;
}


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
   LookupMap::const_iterator it = m_lookupMap.find(LabelAndProduct(iModule.c_str(),iProduct.c_str()));
   assert(it != m_lookupMap.end());
   if(!it->second.wasCached()) {
      GroupHolder waitGroup = it->second.producer()->doProduceAsync();
      dispatch_group_wait(waitGroup.get(), DISPATCH_TIME_FOREVER);
   }
   return it->second.value();
}

int
Event::get(const Getter* iGetter) const
{
  //NOTE: If you have a Getter then we've already prefetched so 
  // this can be done synchronously
  
  LookupMap::const_iterator it = m_lookupMap.find(LabelAndProduct(iGetter->label().c_str(),iGetter->product().c_str()));

  assert(it != m_lookupMap.end());
  auto& gotten = it->second;
  bool wasCached = gotten.wasCached();
  if(not wasCached) {
    std::cout << "Event::get called for missing item "<<iGetter->label()<<" "<<iGetter->product()<<std::endl;
  }
  assert(wasCached);
  return gotten.value();
}


static void getasynchimpl_done_with_get(void* context) {
  dispatch_group_t groupDoneWithGet = static_cast<dispatch_group_t>(context);
  dispatch_group_leave(groupDoneWithGet);
  dispatch_release(groupDoneWithGet);
}

//asynchronously get data
void
Event::getAsyncImpl(Getter* iGetter, GroupHolder iGroupDoneWithGet) const 
{
   //PROBLEM??: Is map find thread safe?  It looks like if I call find via two different threads
   // that I sometimes get the answer for the first thread when I'm running the second thread
   // this causes the wrong producer to be called
   //printf("   Event::getAsyncImp look for %s\n",iGetter->label().c_str());
   LookupMap::const_iterator it = m_lookupMap.find(LabelAndProduct(iGetter->label().c_str(),iGetter->product().c_str()));
   //printf("    found: %s %p\n",it->first.first.c_str(),it->second.first);
   if(it==m_lookupMap.end()) {
     printf("   Event::getAsyncImp failed to find %s '%s'\n",iGetter->label().c_str(),iGetter->product().c_str());
     assert(it != m_lookupMap.end());
     exit(1);
   }
   const DataCache* found = &(it->second);
   if(!found->wasCached()) {
      //printf("     getGroup: %s\n",found->first->label().c_str());
      GroupHolder waitGroup = found->producer()->doProduceAsync();
      //Need to announce when we're done getting the data by leaving the group
      dispatch_group_enter(iGroupDoneWithGet.get());
      dispatch_group_t groupDoneWithGet = iGroupDoneWithGet.get();
      dispatch_retain(groupDoneWithGet);
      dispatch_group_notify_f(waitGroup.get(), s_thread_safe_queue, groupDoneWithGet, getasynchimpl_done_with_get);
   } else {
      //printf("     iGetter->set from already run producer %s %i\n",found->first->label().c_str(),found->second);
      //iGetter->set(found->value());
   }
}

void 
Event::getAsync(Getter* iGetter, GroupHolder iGroupDoneWithGet) const
{
  getAsyncImpl(iGetter, iGroupDoneWithGet);
}

void 
Event::put(const Producer* iProd, const std::string& iProduct, int iPut) {
   LookupMap::iterator itFind = m_lookupMap.find(LabelAndProduct(iProd->label().c_str(),iProduct.c_str()));
   assert(itFind != m_lookupMap.end());
  itFind->second.setValue(iPut);
}


void 
Event::addProducer(Producer* iProd) {
   const std::set<DataKey>& keys = iProd->products();
   for(std::set<DataKey>::const_iterator it = keys.begin(), itEnd = keys.end();
       it != itEnd;
       ++it) {
     m_lookupMap.insert(std::make_pair(LabelAndProduct(iProd->label().c_str(), it->c_str()), DataCache(iProd,this)));
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


static void event_get_suspend_non_thread_safe_queue(void*) {
  dispatch_suspend(s_non_thread_safe_queue);
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
            dispatch_resume(s_non_thread_safe_queue);
         }
         int returnValue = m_event->get(iModule,iProduct);
         if(!m_isThreadSafe) {
            //we must acquire the 'lock' on the non-thread-safe queue again
            dispatch_sync_f(s_non_thread_safe_queue, s_non_thread_safe_queue, event_get_suspend_non_thread_safe_queue);
         }
         return returnValue;
      }
      
      int Event::get(const Getter* iGetter) const {
        return m_event->get(iGetter);
      }
     
      //asynchronously get data. The group will be incremented and will not be 
      // decremented until the attempt to get the data is finished
      void Event::getAsync(Getter* iGetter, GroupHolder iGroup) const {
         m_event->getAsync(iGetter, iGroup);
      }      
   }
}
