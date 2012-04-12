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
#include "Getter.h"
#include "Locks.h"

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
    m_lookupMap.insert(std::make_pair(it->first, DataCache(it->second)));
  }  
}

int 
Event::get(const std::string& iModule, 
           const std::string& iProduct) const
{
   LookupMap::const_iterator it = m_lookupMap.find(std::make_pair(iModule,iProduct));
   assert(it != m_lookupMap.end());
   if(!it->second.wasCached()) {
     it->second.producer()->doProduce(const_cast<Event&>(*this));
   }
   return it->second.value();
}

void
Event::prefetch(const Getter* iGetter) const
{
  LookupMap::const_iterator it = m_lookupMap.find(std::make_pair(iGetter->label(),iGetter->product()));
   assert(it != m_lookupMap.end());
   if(!it->second.wasCached()) {
     it->second.producer()->doProduce(const_cast<Event&>(*this));
   }
   return;
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
    std::cout << "Event::get called for missing item "<<iGetter->label()<<" "<<iGetter->product()<<std::endl;
  }
  assert(wasCached);
  return gotten.value();
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
     m_lookupMap.insert(std::make_pair(std::make_pair(iProd->label(), *it), DataCache(iProd)));
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


namespace {
  class ReleaseAndReaquireLock {
  public:
    ReleaseAndReaquireLock(OMPLock* iLock) :
      m_lock(iLock) 
    {
      if(m_lock) { m_lock->unset();}
    }
    ReleaseAndReaquireLock( ReleaseAndReaquireLock const&) = delete;
    ReleaseAndReaquireLock& operator=(ReleaseAndReaquireLock const&) = delete;

    ~ReleaseAndReaquireLock() 
    {
      if(m_lock) { m_lock->set();}
    }
  private:
    OMPLock* m_lock;
  };
}

namespace demo {
   namespace edm {
      //synchronously get data
      int Event::get(const std::string& iModule, 
                     const std::string& iProduct) const {
	//If the calling module is not thread safe, then it has the thread unsafe lock.
	// We need to temporarily release this so if we must get data from another thread unsafe
	// module we won't get a deadlock
	ReleaseAndReaquireLock sentry( m_isThreadSafe? 0 : s_thread_unsafe_lock.get());
	return m_event->get(iModule,iProduct);
      }
      
      int Event::get(const Getter* iGetter) const {
        return m_event->get(iGetter);
      }
   }
}
