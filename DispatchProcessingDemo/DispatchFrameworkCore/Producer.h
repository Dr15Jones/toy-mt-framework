/*
 *  Producer.h
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#ifndef DispatchProcessingDemo_Producer_h
#define DispatchProcessingDemo_Producer_h

#include <string>
#include <set>
#include <dispatch/dispatch.h>
#import "Module.h"
#import "GroupHolder.h"

namespace demo {
  class Event;
  namespace edm {
    class Event;
  }
  typedef std::string DataKey;
  
  class Producer : public Module {
  public:
    Producer(const std::string& iLabel,
             ThreadType iThreadType= kThreadUnsafe);
    virtual ~Producer();
    
    Producer(const Producer&);
    
    const std::set<DataKey>& products() const {
      return m_products;
    }
    
    void doProduce(Event&);
    
  protected:
    void registerProduct(const DataKey& iProduct) {
      m_products.insert(iProduct);
    }
  private:
    virtual void produce(edm::Event&)=0;
    std::set<DataKey> m_products;
  };
  
}

#endif

