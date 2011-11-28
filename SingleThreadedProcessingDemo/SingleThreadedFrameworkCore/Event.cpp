//
//  Event.cpp
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 8/5/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <cassert>

#import "Event.h"
#import "Producer.h"
#import "Getter.h"

using namespace demo;
typedef std::map<std::pair<std::string,std::string>, std::pair<Producer*,int> > LookupMap;
int 
Event::get(const std::string& iModule, 
           const std::string& iProduct) const
{
  LookupMap::const_iterator it = m_lookupMap.find(std::make_pair(iModule,iProduct));
  assert(it != m_lookupMap.end());
  if(!it->second.first->wasRun()) {
    it->second.first->doProduce(*const_cast<Event*>(this));
  }
  return it->second.second;
}


void 
Event::put(const Producer* iProd, const std::string& iProduct, int iPut) {
  LookupMap::iterator itFind = m_lookupMap.find(std::make_pair(iProd->label(),iProduct));
  assert(itFind != m_lookupMap.end());
  itFind->second.second = iPut;
}


void 
Event::addProducer(Producer* iProd) {
  const std::set<DataKey>& keys = iProd->products();
  for(std::set<DataKey>::const_iterator it = keys.begin(), itEnd = keys.end();
      it != itEnd;
      ++it) {
    m_lookupMap[std::make_pair(iProd->label(),*it)]=std::make_pair(iProd,int(0));
  }
}
void 
Event::reset()
{
  for(LookupMap::iterator it = m_lookupMap.begin(),itEnd=m_lookupMap.end();
      it != itEnd;
      ++it) {
    it->second.first->reset();
  }
}

void Event::get(Getter* iGetter) const
{
  LookupMap::const_iterator it = m_lookupMap.find(std::make_pair(iGetter->label(),iGetter->product()));
  //printf("    found: %s %p\n",it->first.first.c_str(),it->second.first);
  assert(it != m_lookupMap.end());
  const std::pair<Producer*,int>* found = &(it->second);
  if(!found->first->wasRun()) {
    found->first->doProduce(*const_cast<Event*>(this));
    iGetter->set(found->second);
  } else {
    //printf("     iGetter->set from already run producer %s %i\n",found->first->label().c_str(),found->second);
    iGetter->set(found->second);
  }
}

namespace demo {
  namespace edm {
    //synchronously get data
    int Event::get(const std::string& iModule, 
                   const std::string& iProduct) const {
      return m_event->get(iModule,iProduct);
    }
    
    int Event::get(const Getter* iGetter) const {
      return iGetter->value();
    }

  }
}
