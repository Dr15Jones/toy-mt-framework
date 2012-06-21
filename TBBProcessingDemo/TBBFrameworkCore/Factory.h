//
//  Factory.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/29/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_Factory_h
#define DispatchProcessingDemo_Factory_h
#include <boost/property_tree/ptree.hpp>

namespace demo {
  template <typename T>
  class FactoryBase {
  public:
    FactoryBase() {}
    
    virtual std::auto_ptr<T> create(const boost::property_tree::ptree&) const =0;
    
  };
  
  template<typename T, typename U>
  class Factory : public FactoryBase<T> {
  public:
    Factory() {}
    
    virtual std::auto_ptr<T> create(const boost::property_tree::ptree& iPTree) const {
      return std::auto_ptr<T>(new U(iPTree) );
    }
  };
}

#include "FactoryManager.h"
#define DEMO_FACTORY_SYM(x,y) DEMO_FACTORY_SYM2(x,y)
#define DEMO_FACTORY_SYM2(x,y) x ## y

#define DEFINE_DEMO_FACTORY(base,type) \
static demo::FactoryMaker<base,type> DEMO_FACTORY_SYM(s_maker , __LINE__ ) (#type)

#endif
