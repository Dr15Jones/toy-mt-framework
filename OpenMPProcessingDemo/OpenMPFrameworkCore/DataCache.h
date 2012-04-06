//
//  DataCache.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/6/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_DataCache_h
#define DispatchProcessingDemo_DataCache_h
#include <atomic>

#include "ProducerWrapper.h"

namespace demo {
  
  class DataCache
  {
  public:
    DataCache(Producer* iProd):
    m_producer(iProd),
    m_value(0),
    m_wasCached(false) {}
      DataCache():m_producer(static_cast<Producer*>(nullptr)),m_wasCached(false){}
    
    DataCache(const DataCache& iOther):
    m_producer(iOther.m_producer),
    m_value(iOther.m_value),
    m_wasCached(false) {}
    
    bool wasCached() const {
      return m_wasCached;
    }
    void reset() {
      m_wasCached=false;
    }
    void setValue(int iValue) {
      m_value=iValue;
      __sync_synchronize();
      //Need to guarantee that m_wasCached is set only after m_value is set
      // NOTE: under c++11 memory model that should be guaranteed
      m_wasCached = true;
      //must make sure this gets flushed
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
    std::atomic<bool> m_wasCached;
    
  };
}

#endif
