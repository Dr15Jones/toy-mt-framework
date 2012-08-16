/*
 *  Event.h
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef DispatchProcessingDemo_Event_h
#define DispatchProcessingDemo_Event_h

#include <map>
#include <string>
#include "tbb/task.h"
#include "Producer.h"
#include "DataCache.h"

namespace demo {
  
  class Producer;
  class Getter;  
  class Event {
  public:
    Event();
    Event(const Event&);
    unsigned long index() const {return m_index;}
    
    //Used for testing, sets correlated speed of modules
    // value goes from 0. to 1.
    double relativeSpeed() const {return m_relativeSpeed;}
    
    //synchronously get data
    int get(const std::string& iModule, 
            const std::string& iProduct) const;
    
    int get(const Getter*) const;
    
    //asynchronously get data. The task will be decremented once the data has been gotten.
    // If the ref_count of the task reaches zero, it will be spawned.
    void getAsync(Getter* iGetter, tbb::task* iTask) const;
    
    void put(const Producer*, const std::string&, int);
    void setIndex(unsigned long iIndex) {
      m_index = iIndex;
    }
    
    //The following should only be called by EventProcessor
    void reset();
    void addProducer(Producer* iProd);
    void setRelativeSpeed(double iSpeed) {
      m_relativeSpeed = iSpeed;
    }
    
    Event* clone();
    
    struct LabelAndProduct {
      const char*  m_label;
      const char*  m_product;
      LabelAndProduct(const char* iLabel, const char* iProduct ):
      m_label(iLabel),m_product(iProduct) {}
      
      bool operator<(const LabelAndProduct&) const;
    };

  private:
    void getAsyncImpl(Getter* iGetter, tbb::task* iTask) const;
    
    
    std::map<LabelAndProduct,DataCache> m_lookupMap;
    unsigned long m_index;
    double m_relativeSpeed;
  };
  
  namespace edm {
    class Event {
    public:
      Event( demo::Event* iEvent, bool isThreadSafe): 
      m_event(iEvent),
      m_isThreadSafe(isThreadSafe){}
      
      unsigned long index() const {return m_event->index();}
      double relativeSpeed() const { return m_event->relativeSpeed();}
      
      //synchronously get data
      int get(const std::string& iModule, 
              const std::string& iProduct) const;
      
      int get(const Getter*) const;
      
      //asynchronously get data. The task will be decremented once the data has been gotten.
      // If the ref_count of the task reaches zero, it will be spawned.
      void getAsync(Getter* iGetter, tbb::task* iTask) const;
      
      void put(const Producer* iProd, const std::string& iLabel, int iValue) {
        m_event->put(iProd,iLabel,iValue);
      }
    private:
      demo::Event* m_event;
      bool m_isThreadSafe;
    };
  }
}
#endif
