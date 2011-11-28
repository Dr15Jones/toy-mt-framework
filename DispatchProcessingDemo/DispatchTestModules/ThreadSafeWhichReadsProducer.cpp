//
//  ThreadSafeWhichReadsProducer.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#import "ThreadSafeWhichReadsProducer.h"
#import "Event.h"
#import "Getter.h"
#import "busyWait.h"

using namespace demo;

ThreadSafeWhichReadsProducer::ThreadSafeWhichReadsProducer(const std::string& iLabel,
                             const std::string& iProduct,
                             int iValue,
                             const std::vector<std::pair<std::string,std::string> >& iToGet) :
Producer(iLabel,kThreadSafeBetweenModules),
m_product(iProduct),
m_value(iValue) {
  registerProduct(iProduct);
  for(std::vector<std::pair<std::string,std::string> >::const_iterator it = iToGet.begin(),
      itEnd = iToGet.end();
      it != itEnd;
      ++it) {
    m_getters.push_back(registerGet(it->first,it->second));
  }
}

ThreadSafeWhichReadsProducer::ThreadSafeWhichReadsProducer(const ThreadSafeWhichReadsProducer& iOther):
Producer(iOther),
m_product(iOther.m_product),
m_value(iOther.m_value)
{
  m_getters.reserve(iOther.m_getters.size());
  for(std::vector<const Getter*>::const_iterator it = iOther.m_getters.begin(), itEnd=iOther.m_getters.end();
      it != itEnd;
      ++it) {
    m_getters.push_back(registerGet((*it)->label(), 
                                    (*it)->product()));
  }
}

void 
ThreadSafeWhichReadsProducer::produce(edm::Event& iEvent) {
  //printf("Producer %s\n",label().c_str());
  int sum=0;
  for(std::vector<const Getter*>::iterator it = m_getters.begin(), itEnd=m_getters.end();
      it != itEnd;
      ++it) {
    sum +=iEvent.get(*it);
    //printf("%s got %s with value %i\n",label().c_str(), (*it)->label().c_str(), iEvent.get((*it)));
  }
  float v = busyWait(100000);
  iEvent.put(this,m_product,static_cast<int>(m_value+sum+(static_cast<unsigned long long>(v)%10)));
}
