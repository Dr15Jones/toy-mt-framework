//
//  FactoryManager.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/29/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_FactoryManager_h
#define DispatchProcessingDemo_FactoryManager_h
#include <map>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

#include "Factory.h"

namespace demo {
  template<typename T> class FactoryBase;
  
  template <typename T>
  class FactoryManager {
  public:
    static std::auto_ptr<T> create(const std::string& iType, const boost::property_tree::ptree& iConfig) {
      FactoryManager<T>* fm = instance();
      typename std::map<std::string, boost::shared_ptr<FactoryBase<T> > >::iterator itFind = fm->m_factories.find(iType);
      if(itFind != fm->m_factories.end()) {
        return itFind->second->create(iConfig);
      }
      return std::auto_ptr<T>();
    }
    
    static void registerFactory(const std::string& iType, FactoryBase<T>* iFactory) {
      FactoryManager<T>* fm = instance();
      fm->m_factories[iType].reset(iFactory);
    }
    
  private:
    static FactoryManager<T>* instance();
    FactoryManager() {}
    std::map<std::string, boost::shared_ptr<FactoryBase<T> > > m_factories;
  };
  
  template<typename BASE, typename T>
  class FactoryMaker {
  public:
    FactoryMaker(const char* iType) {
      FactoryManager<BASE>::registerFactory(iType, new Factory<BASE,T>());
    }
  };
}

#define CONCATENATE_HIDDEN(a,b) a ## b 
#define CONCATENATE(a,b) CONCATENATE_HIDDEN(a,b)
#define INSTANTIATE_FACTORYMANAGER(TYPE) \
namespace demo {\
 template<> demo::FactoryManager<TYPE>* demo::FactoryManager<TYPE>::instance() {static demo::FactoryManager<TYPE> s_instance; return &s_instance;}\
} enum {CONCATENATE(dummy_demo_instantiate_factorymanager_, __LINE__)}

#endif
