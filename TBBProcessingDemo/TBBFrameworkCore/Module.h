/*
 *  Module.h
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef DispatchProcessingDemo_Module_h
#define DispatchProcessingDemo_Module_h

#include <string>
#include <list>
#include "Getter.h"

namespace tbb {
   class task;
}

namespace demo {
   class Event;
   typedef std::string DataKey;
   class GroupHolder;
   class ModuleWrapper;
   
   enum ThreadType {
     kThreadUnsafe,
     kThreadSafeBetweenModules, //can run simultaneously with other modules but not itself
     kThreadSafeBetweenInstances //multiple events can access the module simultaneously
   };
  
   class Module {
     friend class ModuleWrapper;
   public:
      virtual ~Module();
      
      const std::string& label() const { return m_label;}
      void setLabel(const std::string& iLabel) { m_label = iLabel;}
      ThreadType threadType() const { return m_threadType;}
      bool hasPrefetchItems() const { return !m_getters.empty();}
   protected:
      Module(const std::string& iLabel, ThreadType iThreadType= kThreadUnsafe): 
      m_label(iLabel),
      m_threadType(iThreadType) {}
      const Getter* registerGet(const std::string&, const std::string&);
   private:
      //only called by ModuleWrapper
      void prefetchAsync(const Event&, tbb::task*);
      
      std::string m_label;
      std::list<Getter> m_getters; //returns a pointer so need stable address
      ThreadType m_threadType;
   };
   
}
#endif