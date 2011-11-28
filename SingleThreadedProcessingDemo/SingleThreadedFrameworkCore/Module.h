//
//  Module.h
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 8/5/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef SingleThreadedProcessingDemo_Module_h
#define SingleThreadedProcessingDemo_Module_h
#include <vector>
#include <string>

namespace demo {
  class Event;
  class Getter;
  typedef std::string DataKey;
  
  class Module {
  public:
    virtual ~Module();
    
    const std::string& label() const { return m_label;}
    void setLabel(const std::string& iLabel) { m_label = iLabel;}
    void prefetch(const Event&);
    bool wasRun() const {return m_wasRun;}
    void reset() { m_wasRun = false;}
    bool hasPrefetchItems() const { return !m_getters.empty();}
  protected:
    Module(const std::string& iLabel):
    m_label(iLabel) {}
    const Getter* registerGet(const std::string&, const std::string&);
    void setWasRun() { m_wasRun=true;}
  private:
    std::string m_label;
    std::vector<Getter*> m_getters;
    bool m_wasRun;
  };
  
}

#endif
