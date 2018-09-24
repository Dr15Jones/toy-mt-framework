//
//  ThreadSafeWhichReadsProducer.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ThreadSafeWhichReadsProducer_h
#define DispatchProcessingDemo_ThreadSafeWhichReadsProducer_h
#include <string>
#include <vector>

#include "Producer.h"

namespace demo {
  class Getter;
  
  class ThreadSafeWhichReadsProducer : public Producer {
  public:
    ThreadSafeWhichReadsProducer(const std::string& iLabel,
                                 const std::string& iProduct,
                                 int iValue,
                                 const std::vector<std::pair<std::string,std::string> >& iToGet);
    ThreadSafeWhichReadsProducer(const ThreadSafeWhichReadsProducer& iOther);
    
  private:
    void produce(edm::Event& iEvent);
    std::string m_product;
    int m_value;
    std::vector<const Getter*> m_getters;
  };

}
#endif
