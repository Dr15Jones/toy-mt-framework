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
#include <atomic>

#include "Event.h"
#include "Producer.h"
#include "ProducerWrapper.h"
#include "Queues.h"
#include "Getter.h"

using namespace demo;
typedef std::map<Event::LabelAndProduct, DataCache > LookupMap;

static std::atomic<unsigned int> s_nextThreadID{0};
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
m_relativeSpeed(1),
m_transitionID(s_nextThreadID++)
{}

Event::Event(const Event& iOther):
m_index(iOther.m_index),
m_relativeSpeed(iOther.m_relativeSpeed),
m_transitionID(s_nextThreadID++)
{
  m_producers.resize(iOther.m_producers.size());
  //NOTE: Items in the DataCache have references to the Event
  // so we need our own copy of them
  for(LookupMap::const_iterator it = iOther.m_lookupMap.begin(),itEnd=iOther.m_lookupMap.end();
      it != itEnd;
      ++it) {
    m_lookupMap.insert(std::make_pair(it->first, DataCache(it->second, this)));
    auto prod = m_lookupMap[it->first].producer();
    m_producers[prod->id()]=prod;
  }  
}

int 
Event::get(const std::string& iModule, 
           const std::string& iProduct) const
{
   LookupMap::const_iterator it = m_lookupMap.find(LabelAndProduct(iModule.c_str(),iProduct.c_str()));
   assert(it != m_lookupMap.end());
   if(!it->second.wasCached()) {
      tbb::empty_task* doneTask = new (tbb::task::allocate_root()) tbb::empty_task();
      //Ref count needs to be 1 since WaitList will increment and decrement it again
      // doneTask->wait_for_all() waits for the task ref count to drop back to 1
      doneTask->increment_ref_count();
      it->second.producer()->doProduceAsync(doneTask);
      //fprintf(stderr,"Waiting to get %s\n",iModule.c_str());
      doneTask->wait_for_all();
      //fprintf(stderr,"Done waiting to get %s\n",iModule.c_str());
      tbb::task::destroy(*doneTask);
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
   LookupMap::const_iterator it = m_lookupMap.find(LabelAndProduct(iGetter->label().c_str(),iGetter->product().c_str()));
   //printf("    found: %s %s\n",it->first.m_label,it->first.m_product);
   if(it==m_lookupMap.end()) {
     printf("   Event::getAsyncImp failed to find %s '%s'\n",iGetter->label().c_str(),iGetter->product().c_str());
     assert(it != m_lookupMap.end());
     exit(1);
   }
   const DataCache* found = &(it->second);
   //printf("     doProduceAsync: %s\n",iGetter->label().c_str());
   //NOTE: this will not rerun the producer if it has already been run AND it will
   // properly run iTaskDoneWithGet in both cases (producer run or not run yet)
   found->producer()->doProduceAsync(iTaskDoneWithGet);
}

void Event::mustWaitFor(unsigned int iModuleID, tbb::task* iTask) const {
   //std::cout <<"mustWaitFor "<<iModuleID<<" size "<<m_producers.size()<<std::endl;
   assert(iModuleID < m_producers.size());
   m_producers[iModuleID]->doProduceAsync(iTask);
}


void 
Event::getAsync(Getter* iGetter, tbb::task* iTaskDoneWithGet) const
{
  getAsyncImpl(iGetter, iTaskDoneWithGet);
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
     LabelAndProduct key{LabelAndProduct(iProd->label().c_str(), it->c_str())};
     auto prodIt = m_lookupMap.insert(std::make_pair(key, DataCache(iProd,this)));
     auto prod = prodIt.first->second.producer();
     assert(prod);
     //NOTE: calling prod->id() always gives 0 even though it is supposed to be forwarding to iProd->id() !!
     if(m_producers.size()<=iProd->id()) {
        m_producers.resize(iProd->id()+1);
     }
     assert(iProd->id() < m_producers.size());
     m_producers[iProd->id()]=prod;
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
           tbb::empty_task* doneTask = new (tbb::task::allocate_root()) tbb::empty_task();
           //Ref count needs to be w since task willdecrement it again once we've aquired
           // the lock and  doneTask->wait_for_all() waits for the task ref count
           // to drop back to 1
           doneTask->increment_ref_count();
           doneTask->increment_ref_count();
           s_non_thread_safe_queue->push([doneTask]{
             s_non_thread_safe_queue->pause();
             doneTask->decrement_ref_count();
           });
           doneTask->wait_for_all();
           tbb::task::destroy(*doneTask);
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
