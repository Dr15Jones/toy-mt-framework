//
//  Event_test.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/27/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>

#include <cppunit/extensions/HelperMacros.h>
#include <unistd.h>
#include <memory>
#include <atomic>
#include "Event.h"
#include "Producer.h"
#include "SerialTaskQueue.h"
#include "tbb/task.h"
#include "tbb/parallel_for.h"

class Event_test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(Event_test);
  CPPUNIT_TEST(getSyncDirect);
  CPPUNIT_TEST(getSyncIndirect);
  CPPUNIT_TEST(getSyncIndirectWithGetter);
  CPPUNIT_TEST(callOnce);
  CPPUNIT_TEST(simultaneousBetweenInstancesOneEvent);
  CPPUNIT_TEST(simultaneousBetweenModulesOneEvent);
  CPPUNIT_TEST(simultaneousThreadUnsafeOneEvent);
  CPPUNIT_TEST(simultaneousBetweenInstancesTwoEvents);
  CPPUNIT_TEST(simultaneousBetweenModulesTwoEvents);
  CPPUNIT_TEST(simultaneousThreadUnsafeTwoEvents);
  CPPUNIT_TEST_SUITE_END();
  
public:
  void getSyncDirect();
  void getSyncIndirect();
  void getSyncIndirectWithGetter();
  void callOnce();
  void simultaneousBetweenModulesOneEvent();
  void simultaneousBetweenInstancesOneEvent();
  void simultaneousThreadUnsafeOneEvent();
  void simultaneousBetweenModulesTwoEvents();
  void simultaneousBetweenInstancesTwoEvents();
  void simultaneousThreadUnsafeTwoEvents();
  void setUp(){}
  void tearDown(){}
};

namespace  {
  class ValueProd : public demo::Producer {
  public:
    ValueProd(const std::string& iLabel, const std::string& iProduct, int iValue):
    demo::Producer(iLabel,demo::kThreadSafeBetweenInstances),
    m_product(iProduct),
    m_value(iValue){
      registerProduct(demo::DataKey(iProduct));
    }
  private:
    virtual void produce(demo::edm::Event& iEvent) {
      iEvent.put(this, m_product, m_value);
    }
    std::string m_product;
    int m_value;
  };

  class SumGetterProd : public demo::Producer {
  public:
    SumGetterProd(const std::string& iLabel, const std::string& iProduct, const std::vector<std::pair<std::string,std::string> >& iToGet):
    demo::Producer(iLabel,demo::kThreadSafeBetweenInstances),
    m_product(iProduct)
    {
      registerProduct(demo::DataKey(iProduct));
      for(const std::pair<std::string,std::string>& x : iToGet) {
        m_getters.push_back(registerGet(x.first, x.second));
      }
    }
  private:
    virtual void produce(demo::edm::Event& iEvent) {
      int value = 0;
      for(const demo::Getter* g: m_getters) {
        value += iEvent.get(g);
      }
      iEvent.put(this, m_product, value);
    }
    std::string m_product;
    std::vector<const demo::Getter*> m_getters;
  };

  class SumSyncProd : public demo::Producer {
  public:
    SumSyncProd(const std::string& iLabel, const std::string& iProduct, const std::vector<std::pair<std::string,std::string> >& iToGet):
    demo::Producer(iLabel,demo::kThreadSafeBetweenInstances),
    m_product(iProduct),
    m_toGet(iToGet)
    {
      registerProduct(demo::DataKey(iProduct));
    }
  private:
    virtual void produce(demo::edm::Event& iEvent) {
      int value = 0;
      for(const std::pair<std::string,std::string>& g: m_toGet) {
        value += iEvent.get(g.first,g.second);
      }
      iEvent.put(this, m_product, value);
    }
    std::string m_product;
    const std::vector<std::pair<std::string,std::string> > m_toGet;
  };
  
  class ThreadSafeCounter {
  public:
    ThreadSafeCounter(const char* iName):
    m_count(0){}
    
    int increment() {
       return ++m_count;
    }
    
    int value() {
       return m_count;
    }
    
    ~ThreadSafeCounter() {
    }
  private:
    
    std::atomic<int> m_count;
  };

  class AccessTestingProd : public demo::Producer {
  public:
    AccessTestingProd(const std::string& iLabel, const std::string& iProduct, demo::ThreadType iThreadType, ThreadSafeCounter& iCounter, bool& iWasChanged):
    Producer(iLabel,iThreadType),
    m_product(iProduct),
    m_counter(&iCounter),
    m_wasChanged(&iWasChanged){
      registerProduct(demo::DataKey(iProduct));
    }
  private:
    virtual void produce(demo::edm::Event& iEvent) {
      int newV = m_counter->increment();
      sleep(1);
      if(newV != m_counter->value()) {
        *m_wasChanged =true;
      }
      iEvent.put(this, m_product, 1);
    }
    std::string m_product;
    ThreadSafeCounter* m_counter;
    bool* m_wasChanged;
  };
}

void Event_test::getSyncDirect()
{
  ValueProd* one(new ValueProd("one","one",1));
  ValueProd* two(new ValueProd("two","two",2));
  
  demo::Event event;
  event.setIndex(1);
  
  CPPUNIT_ASSERT(event.index()==1);
  
  event.addProducer(one);
  event.addProducer(two);
  
  CPPUNIT_ASSERT(event.get("one","one")==1);
  CPPUNIT_ASSERT(event.get("two","two")==2);
  
}

void Event_test::getSyncIndirect()
{
  ValueProd* one(new ValueProd("one","one",1));
  ValueProd* two(new ValueProd("two","two",2));
  std::vector<std::pair<std::string, std::string> > toGet;
  
  toGet.push_back(std::pair<std::string,std::string>("one","one"));
  toGet.push_back(std::pair<std::string,std::string>("two","two"));
  SumSyncProd* sum(new SumSyncProd("sum","",toGet));
  
  demo::Event event;
  event.setIndex(1);
  
  CPPUNIT_ASSERT(event.index()==1);
  
  event.addProducer(one);
  event.addProducer(two);
  event.addProducer(sum);
  
  CPPUNIT_ASSERT(event.get("sum","")==3);
  
}

void Event_test::getSyncIndirectWithGetter()
{
  ValueProd* one(new ValueProd("one","one",1));
  ValueProd* two(new ValueProd("two","two",2));
  std::vector<std::pair<std::string, std::string> > toGet;
  
  toGet.push_back(std::pair<std::string,std::string>("one","one"));
  toGet.push_back(std::pair<std::string,std::string>("two","two"));
  SumGetterProd* sum(new SumGetterProd("sum","",toGet));
  
  demo::Event event;
  event.setIndex(1);
  
  CPPUNIT_ASSERT(event.index()==1);
  
  event.addProducer(one);
  event.addProducer(two);
  event.addProducer(sum);
  
  CPPUNIT_ASSERT(event.get("sum","")==3);
  
}

void Event_test::callOnce()
{
  ThreadSafeCounter count("gov.fnal.counter");
  bool wasChanged=false;
  
  AccessTestingProd* at(new AccessTestingProd("access","",demo::kThreadSafeBetweenInstances,count,wasChanged));
  std::vector<std::pair<std::string, std::string> > toGet;
  toGet.push_back(std::pair<std::string,std::string>("access",""));
  SumSyncProd* sum1(new SumSyncProd("sum1","",toGet));
  SumSyncProd* sum2(new SumSyncProd("sum2","",toGet));

  toGet.clear();
  toGet.push_back(std::pair<std::string,std::string>("sum1",""));
  toGet.push_back(std::pair<std::string,std::string>("sum2",""));
  SumSyncProd* sum3(new SumSyncProd("sum3","",toGet));

  demo::Event event;
  event.setIndex(1);
  
  event.addProducer(at);
  event.addProducer(sum1);
  event.addProducer(sum2);
  event.addProducer(sum3);
  
  CPPUNIT_ASSERT(event.get("sum3","")==2);
  CPPUNIT_ASSERT(count.value()==1);
  CPPUNIT_ASSERT(wasChanged==false);
}

void Event_test::simultaneousBetweenInstancesOneEvent()
{
  ThreadSafeCounter count("gov.fnal.counter");
  bool wasChanged=false;
  
  AccessTestingProd* at1(new AccessTestingProd("access1","",demo::kThreadSafeBetweenInstances,count,wasChanged));
  AccessTestingProd* at2(new AccessTestingProd("access2","",demo::kThreadSafeBetweenInstances,count,wasChanged));
  std::vector<std::pair<std::string, std::string> > toGet;
  toGet.push_back(std::pair<std::string,std::string>("access1",""));
  toGet.push_back(std::pair<std::string,std::string>("access2",""));
  SumGetterProd* sum3(new SumGetterProd("sum3","",toGet));
  
  demo::Event event;
  event.setIndex(1);
  
  event.addProducer(at1);
  event.addProducer(at2);
  event.addProducer(sum3);
  
  CPPUNIT_ASSERT(event.get("sum3","")==2);
  CPPUNIT_ASSERT(count.value()==2);
  CPPUNIT_ASSERT(wasChanged==true);
}

void Event_test::simultaneousBetweenModulesOneEvent()
{
  ThreadSafeCounter count("gov.fnal.counter");
  bool wasChanged=false;
  
  AccessTestingProd* at1(new AccessTestingProd("access1","",demo::kThreadSafeBetweenModules,count,wasChanged));
  AccessTestingProd* at2(new AccessTestingProd("access2","",demo::kThreadSafeBetweenModules,count,wasChanged));
  std::vector<std::pair<std::string, std::string> > toGet;
  toGet.push_back(std::pair<std::string,std::string>("access1",""));
  toGet.push_back(std::pair<std::string,std::string>("access2",""));
  SumGetterProd* sum3(new SumGetterProd("sum3","",toGet));
  
  demo::Event event;
  event.setIndex(1);
  
  event.addProducer(at1);
  event.addProducer(at2);
  event.addProducer(sum3);
  
  CPPUNIT_ASSERT(event.get("sum3","")==2);
  CPPUNIT_ASSERT(count.value()==2);
  CPPUNIT_ASSERT(wasChanged==true);
}

void Event_test::simultaneousThreadUnsafeOneEvent()
{
  ThreadSafeCounter count("gov.fnal.counter");
  bool wasChanged=false;
  
  AccessTestingProd* at1(new AccessTestingProd("access1","",demo::kThreadUnsafe,count,wasChanged));
  AccessTestingProd* at2(new AccessTestingProd("access2","",demo::kThreadUnsafe,count,wasChanged));
  std::vector<std::pair<std::string, std::string> > toGet;
  toGet.push_back(std::pair<std::string,std::string>("access1",""));
  toGet.push_back(std::pair<std::string,std::string>("access2",""));
  SumGetterProd* sum3(new SumGetterProd("sum3","",toGet));
  
  demo::Event event;
  event.setIndex(1);
  
  event.addProducer(at1);
  event.addProducer(at2);
  event.addProducer(sum3);
  
  CPPUNIT_ASSERT(event.get("sum3","")==2);
  CPPUNIT_ASSERT(count.value()==2);
  CPPUNIT_ASSERT(wasChanged==false);
}

void Event_test::simultaneousBetweenInstancesTwoEvents()
{
  ThreadSafeCounter count("gov.fnal.counter");
  bool wasChanged=false;
  
  AccessTestingProd* at( new AccessTestingProd("access","",demo::kThreadSafeBetweenInstances,count,wasChanged));
  
  demo::Event event1;
  event1.setIndex(1);
  
  event1.addProducer(at);
  
  std::unique_ptr<demo::Event> event2(event1.clone());
  event2->setIndex(2);

  const demo::Event* events[2]={&event1,event2.get()};

  tbb::parallel_for(0,2,[events](int iIndex) {
     events[iIndex]->get("access","");
  });
  CPPUNIT_ASSERT(count.value()==2);
  CPPUNIT_ASSERT(wasChanged==true);
}

void Event_test::simultaneousBetweenModulesTwoEvents()
{
  ThreadSafeCounter count("gov.fnal.counter");
  bool wasChanged=false;
  
  AccessTestingProd* at(new AccessTestingProd("access","",demo::kThreadSafeBetweenModules,count,wasChanged));
  
  demo::Event event1;
  event1.setIndex(1);
  
  event1.addProducer(at);
  std::unique_ptr<demo::Event> event2(event1.clone());
  event2->setIndex(2);

  const demo::Event* events[2]={&event1,event2.get()};
  
  tbb::parallel_for(0,2,[events](int iIndex) {
     events[iIndex]->get("access","");
  });
  CPPUNIT_ASSERT(count.value()==2);
  CPPUNIT_ASSERT(wasChanged==false);
}

void Event_test::simultaneousThreadUnsafeTwoEvents()
{
  ThreadSafeCounter count("gov.fnal.counter");
  bool wasChanged=false;
  
  AccessTestingProd* at(new AccessTestingProd("access","",demo::kThreadUnsafe,count,wasChanged));
  
  demo::Event event1;
  event1.setIndex(3);
  
  event1.addProducer(at);
  std::unique_ptr<demo::Event> event2(event1.clone());
  event2->setIndex(4);

  const demo::Event* events[2]={&event1,event2.get()};
  
  tbb::parallel_for(0,2,[events](int iIndex) {
     events[iIndex]->get("access","");
  });
  CPPUNIT_ASSERT(count.value()==2);
  CPPUNIT_ASSERT(wasChanged==false);
}


CPPUNIT_TEST_SUITE_REGISTRATION( Event_test );