//
//  DataCache.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/6/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_DataCache_h
#define DispatchProcessingDemo_DataCache_h

#include "ProducerWrapper.h"

namespace demo {
  
  class DataCache
  {
  public:
    DataCache(Producer* iProd,Event* iEvent):
    m_producer(iProd, iEvent),
    m_value(0),
    m_wasCached(false) {}
    DataCache():m_producer(0,0),m_wasCached(false){}
    
    DataCache(const DataCache& iOther, Event* iEvent):
    m_producer(iOther.m_producer,iEvent),
    m_value(iOther.m_value),
    m_wasCached(false) {}
    
    bool wasCached() const {
      return m_wasCached;
    }
    void reset() {
      m_wasCached=false;
    }
    void setValue(int iValue) {
      m_wasCached = true;
      m_value=iValue;
    }
    
    ProducerWrapper* producer() const {
      return &m_producer;
    }
    int value() const {
      return m_value;
    }
  private:
    mutable ProducerWrapper m_producer;
    int m_value;
    bool m_wasCached;
    
  };
}

#endif
