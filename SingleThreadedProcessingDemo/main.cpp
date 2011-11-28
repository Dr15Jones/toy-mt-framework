//
//  main.cpp
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 8/5/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <math.h>
#include <assert.h>
#include <cmath>
#include <sys/time.h>
#include <errno.h>
#include <boost/property_tree/json_parser.hpp>


#import "Event.h"
#import "Module.h"
#include "Filter.h"
#import "Producer.h"
#import "Getter.h"
#include "Source.h"
#include "Path.h"
#include "ErrorConditions.h"
#include "Schedule.h"
#include "EventProcessor.h"
#include "FactoryManager.h"


#include "busy_wait_scale_factor.h"


namespace demo {
  class SimpleSource : public Source {
  public:
    SimpleSource(unsigned int iNEvents): 
    m_nEvents(iNEvents),
    m_seenEvents(0) {}
    
    virtual bool setEventInfo(Event& iEvent) {
      //printf("event #%u\n",static_cast<unsigned int>(m_seenEvents));
      iEvent.setIndex(m_seenEvents);
      return m_nEvents >= ++m_seenEvents;
    }
  private:
    unsigned int m_nEvents;
    unsigned int m_seenEvents;
  };
    
}



int main (int argc, char * const argv[]) {
  /*
  
  const size_t iterations= 3000;
  //const size_t iterations= 2;
  demo::EventProcessor ep;
  ep.setSource(new demo::SimpleSource( iterations) );
  demo::Path* p = new demo::Path();
  p->addFilter(new demo::PassFilter("first"));
  p->addFilter(new demo::PassFilter("second"));
  p->addFilter(new demo::PassFilter("third"));
  
  std::vector<std::pair<std::string,std::string> > itemsToGet;
  itemsToGet.push_back(std::make_pair(std::string("alpha"),std::string("a")));
  itemsToGet.push_back(std::make_pair(std::string("beta"),std::string("b")));
  p->addFilter(new demo::GetterFilter("fourth",itemsToGet));
  ep.addPath(p);
  
  ep.addProducer(new demo::WaitingProducer("alpha","a",5));
  ep.addProducer(new demo::WaitingProducer("beta","b",2));
  ep.processAll();
  return 0;
   */
  
  assert(argc==2);
  boost::property_tree::ptree pConfig; 
  try {
    read_json(argv[1],pConfig);
  } catch(const std::exception& iE) {
    std::cout <<iE.what()<<std::endl;
    exit(1);
  }
  //const size_t iterations= 3000;
  const size_t iterations = pConfig.get<size_t>("process.source.iterations");
  //const size_t iterations= 2;
  demo::EventProcessor ep;
  ep.setSource(new demo::SimpleSource( iterations) );
  
  
  busy_wait_scale_factor = pConfig.get<double>("process.options.busyWaitScaleFactor",2.2e+07);
  
  boost::property_tree::ptree& filters=pConfig.get_child("process.filters");
  for(const boost::property_tree::ptree::value_type& f : filters) {
    std::cout << f.second.get<std::string>("@type")<<std::endl;
    std::auto_ptr<demo::Filter> pF = demo::FactoryManager<demo::Filter>::create(f.second.get<std::string>("@type"),f.second);
    if(pF.get() != 0) {
      ep.addFilter(pF.release());
    } else {
      assert(0=="Failed creating filter");
      exit(1);
    }
  }
  
  boost::property_tree::ptree& producers=pConfig.get_child("process.producers");
  for(const boost::property_tree::ptree::value_type& p : producers) {
    std::cout << p.second.get<std::string>("@type")<<std::endl;
    std::auto_ptr<demo::Producer> pP = demo::FactoryManager<demo::Producer>::create(p.second.get<std::string>("@type"),p.second);
    if(pP.get() != 0) {
      ep.addProducer(pP.release());
    } else {
      assert(0=="Failed creating filter");
      exit(1);
    }
  }
  
  
  boost::property_tree::ptree& paths=pConfig.get_child("process.paths");
  for(const boost::property_tree::ptree::value_type& p : paths) {
    std::string n = p.first;
    std::vector<std::string> modules;
    for(const boost::property_tree::ptree::value_type& m: p.second) {
      //std::cout <<m.first<<" - "<<m.second.data()<<std::endl;
      modules.push_back(m.second.data());
    }
    ep.addPath(n,modules);
  }
  
  {
    struct timeval startCPUTime;
    
    rusage theUsage;
    if(0 != getrusage(RUSAGE_SELF, &theUsage)) {
      std::cerr<<errno;
      return 1;
    }
    startCPUTime.tv_sec =theUsage.ru_stime.tv_sec+theUsage.ru_utime.tv_sec;
    startCPUTime.tv_usec =theUsage.ru_stime.tv_usec+theUsage.ru_utime.tv_usec;
    
    struct timeval startRealTime;
    gettimeofday(&startRealTime, 0);
    
    ep.processAll();
    
    struct timeval tp;
    gettimeofday(&tp, 0);
    
    if(0 != getrusage(RUSAGE_SELF, &theUsage)) {
      std::cerr<<errno;
      return 1;
    }
    
    
    double const microsecToSec = 1E-6;
    
    double cpuTime = theUsage.ru_stime.tv_sec + theUsage.ru_utime.tv_sec - startCPUTime.tv_sec +
    microsecToSec * (theUsage.ru_stime.tv_usec + theUsage.ru_utime.tv_usec - startCPUTime.tv_usec);
    
    double realTime = tp.tv_sec - startRealTime.tv_sec + microsecToSec * (tp.tv_usec - startRealTime.tv_usec);
    std::cout <<" total # events:"<<iterations<<" cpu time:" << cpuTime<<" real time:"<<realTime<<" events/sec:"<<iterations/realTime<<std::endl;
    
  }
  

}

