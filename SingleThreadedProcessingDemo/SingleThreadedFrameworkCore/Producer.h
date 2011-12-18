//
//  Producer.h
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 8/5/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef SingleThreadedProcessingDemo_Producer_h
#define SingleThreadedProcessingDemo_Producer_h

#include <string>
#include <set>
#include "Module.h"

namespace demo {
  class Event;
  namespace edm {
    class Event;
  }
  typedef std::string DataKey;
  
  class Producer : public Module {
  public:
    Producer(const std::string& iLabel);
    virtual ~Producer();
    
    Producer(const Producer&);
    
    void doProduce(Event&);
    
    const std::set<DataKey>& products() const {
      return m_products;
    }
    
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
