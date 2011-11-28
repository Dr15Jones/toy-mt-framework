/*
 *  Getter.h
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef DispatchProcessingDemo_Getter_h
#define DispatchProcessingDemo_Getter_h

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
   private:
      
      std::string m_label;
      std::string m_product;
   };
}

#endif