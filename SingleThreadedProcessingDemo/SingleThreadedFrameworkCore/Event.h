//
//  Event.h
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 8/5/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef SingleThreadedProcessingDemo_Event_h
#define SingleThreadedProcessingDemo_Event_h

#include <map>
#include <string>
#include "Producer.h"

namespace demo {
  
  class Producer;
  class Getter;
  
  class Event {
  public:
    unsigned long index() const {return m_index;}
    
    //synchronously get data
    int get(const std::string& iModule, 
            const std::string& iProduct) const;
    
    void get(Getter* iGetter) const;
    
    void put(const Producer*, const std::string&, int);
    void setIndex(unsigned long iIndex) {
      m_index = iIndex;
    }
    
    //Used for testing, sets correlated speed of modules
    // value goes from 0. to 1.
    double relativeSpeed() const {return m_relativeSpeed;}

    
    //The following should only be called by EventProcessor
    void reset();
    void addProducer(Producer* iProd);
    void setRelativeSpeed(double iSpeed) {
      m_relativeSpeed = iSpeed;
    }

    struct LabelAndProduct {
      const char*  m_label;
      const char*  m_product;
      LabelAndProduct(const char* iLabel, const char* iProduct ):
      m_label(iLabel),m_product(iProduct) {}
      
      bool operator<(const LabelAndProduct&) const;
    };

  private:
    
    std::map<LabelAndProduct,std::pair<Producer*,int> > m_lookupMap;
    unsigned long m_index;
    double m_relativeSpeed;
  };
  
  namespace edm {
    class Event {
    public:
      Event( demo::Event* iEvent): 
      m_event(iEvent){}
      
      unsigned long index() const {return m_event->index();}
      double relativeSpeed() const { return m_event->relativeSpeed();}

      
      //synchronously get data
      int get(const std::string& iModule, 
              const std::string& iProduct) const;
      
      int get(const Getter* iGetter) const;
      
      void put(const Producer* iProd, const std::string& iLabel, int iValue) {
        m_event->put(iProd,iLabel,iValue);
      }
    private:
      demo::Event* m_event;
    };
  }
}

#endif
