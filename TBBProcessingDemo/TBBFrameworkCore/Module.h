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
#include <vector>
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
      const Getter* registerGet(const std::string&, const std::string&);
      bool hasPrefetchItems() const { return !m_getters.empty();}
      bool hasMightGetItems() const { return !m_mightGetters.empty();}
      const std::vector<Getter>& mightGet() const {return m_mightGetters;}
      const std::list<Getter>& prefetchItems() const { return m_getters;}
      unsigned int id() const { return m_moduleID;}
      void setID(unsigned int iID) {m_moduleID = iID;}
   protected:
      Module(const std::string& iLabel, ThreadType iThreadType= kThreadUnsafe): 
      m_label(iLabel),
      m_threadType(iThreadType) {}
      void registerMightGet(const std::string&, const std::string&);
   private:
      //only called by ModuleWrapper
      void prefetchAsync(const Event&, tbb::task*);
      
      std::string m_label;
      std::list<Getter> m_getters; //returns a pointer so need stable address
      std::vector<Getter> m_mightGetters;
      ThreadType m_threadType;
      unsigned int m_moduleID; //unique id for the module in this job
   };
   
}
#endif