//
//  Getter.h
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 8/5/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef SingleThreadedProcessingDemo_Getter_h
#define SingleThreadedProcessingDemo_Getter_h

#include <string>

namespace demo {
  typedef std::string DataKey;
  class Event;
  
  class Getter {
  public:
    Getter(const std::string& iLabel,
           const std::string& iProduct);
    const std::string& label() const {return m_label;}
    const std::string& product() const {return m_product;}
    void set(int iValue) { m_value= iValue;}
    int value() const { return m_value;}
  private:
    
    const std::string m_label;
    const std::string m_product;
    int m_value;
  };
}

#endif
