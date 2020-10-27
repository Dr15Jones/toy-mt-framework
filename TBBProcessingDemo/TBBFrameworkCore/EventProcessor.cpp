//
//  EventProcessor.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <cassert>
#include "EventProcessor.h"
#include "Schedule.h"
#include "Event.h"
#include "Path.h"
#include "Source.h"
#include "Producer.h"
#include "Filter.h"

using namespace demo;

EventProcessor::EventProcessor():
  m_source(),
  m_nextModuleID(0),
  m_fatalJobErrorOccured(false)
  {
    m_schedules.emplace_back(std::make_unique<Schedule>());
    m_schedules[0]->setFatalJobErrorOccurredPointer(&m_fatalJobErrorOccured);
    
  }

EventProcessor::~EventProcessor()
{
}

void 
EventProcessor::setSource(Source* iSource) {
  m_source.reset(iSource);
}
void
EventProcessor::addPath(const std::string& iName,
                        const std::vector<std::string>& iModules) {
  m_schedules[0]->addPath(iModules);
}

void 
EventProcessor::addProducer(Producer* iProd) {
  iProd->setID(m_nextModuleID);
  ++m_nextModuleID;
  m_producers[iProd->label()]=iProd;
  m_schedules[0]->event()->addProducer(iProd);
}

void
EventProcessor::addFilter(Filter* iFilter) {
  iFilter->setID(m_nextModuleID);
  ++m_nextModuleID;
  m_filters.push_back(iFilter);
  m_schedules[0]->addFilter(iFilter);
}

static
bool alternateDependencyPaths(unsigned int iParentID,
                              unsigned int iChildID,
                              std::map<unsigned int, std::vector<unsigned int>> const& iDeps) {
   auto itFound = iDeps.find(iChildID);
   if(itFound!=iDeps.end()) {
     for(auto& parent: itFound->second ) {
       if(parent != iParentID) {
         if(alternateDependencyPaths(iParentID,parent,iDeps)) {
           return true;
         }
       }
     }
   } else {
     return true;
   }
   return false;
}

static
void recursivelyCheckGetItems(unsigned int iStartID, 
                                   Module* iModule, 
                                   std::map<std::string,Producer*> const& iProds,
                                   std::set<unsigned int> const& iAlreadyPrefetched,
                                   std::map<unsigned int,std::vector<unsigned int>> const& iDeps,
                                   std::set<unsigned int>& oToCheck){
  //Items with no dependencies in the demo represent reading from storage. Storage is always OK
  if(iModule->mightGet().empty() and iModule->prefetchItems().empty()) { return; }
  
  if(oToCheck.find(iModule->id())!=oToCheck.end()) {
     //alread have it so do not have to probe further
     return;
  }
  //search back up the inheritance hierarchy. If the only way to reach iModule is to go through iStartID
  // then we can keep this as 'mightGet'.
  if(iAlreadyPrefetched.find(iModule->id())==iAlreadyPrefetched.end() and alternateDependencyPaths(iStartID,iModule->id(),iDeps)) {
     std::cout <<"Avoid deadlock: "<<iStartID<<" will look for "<<iModule->label()<<std::endl;

     oToCheck.insert(iModule->id());
  }
  for(auto const& getter: iModule->mightGet()) {
     auto it = iProds.find(getter.label());
     recursivelyCheckGetItems(iStartID,it->second,iProds,iAlreadyPrefetched,iDeps,oToCheck);        
  }
  for(auto const& getter: iModule->prefetchItems()) {
    auto it = iProds.find(getter.label());
    recursivelyCheckGetItems(iStartID,it->second,iProds,iAlreadyPrefetched,iDeps,oToCheck);
  }
  return;
}

static
void recursivelyCheckPrefetchItems(Module* iModule, 
                                   std::map<std::string,Producer*> const& iProds,
                                   std::set<unsigned int>& iDeps){
  if(iDeps.find(iModule->id())!=iDeps.end()) {
     return;
  }
  if(iModule->hasPrefetchItems()) {
     for(auto const& getter: iModule->prefetchItems()) {
        auto it = iProds.find(getter.label());
        iDeps.insert(it->second->id());
        recursivelyCheckPrefetchItems(it->second,iProds,iDeps);
     }
  }
  return;
}

static
void correctPossibleDeadlocks(Module* iModule, 
                              std::map<std::string,Producer*> const& iProds,
                              std::map<unsigned int,std::vector<unsigned int>> const& iDeps) {
  std::set<unsigned int> modulesAlreadyPrefetching;
  recursivelyCheckPrefetchItems(iModule,iProds,modulesAlreadyPrefetching);
  
  std::set<unsigned int> modulesToCheck;
  if(iModule->hasMightGetItems()) {
    for(auto const& getter: iModule->mightGet()) {
      auto it = iProds.find(getter.label());
      assert(it != iProds.end());
      recursivelyCheckGetItems(iModule->id(), it->second,iProds,modulesAlreadyPrefetching,iDeps,modulesToCheck);
    }
  }
  std::vector<unsigned int> toCheck{modulesToCheck.begin(),modulesToCheck.end()};
  iModule->setDependentModuleToCheck(toCheck);
}

static
void addDependencies(Module* iModule, std::map<std::string, Producer*> const& iProds, std::map<unsigned int,std::vector<unsigned int>>& iDeps) {
  for(auto const& getter: iModule->prefetchItems()) {
    auto it = iProds.find(getter.label());
    iDeps[it->second->id()].push_back(iModule->id());
  }
  for(auto const& getter: iModule->mightGet()) {
    auto it = iProds.find(getter.label());
    iDeps[it->second->id()].push_back(iModule->id());
  }
}



void 
EventProcessor::finishSetup() {
  //Look for cases where a 'might get' could lead to a deadlock. In those
  // cases convert the 'might get' to a 'prefetch'.
  
  std::map<unsigned int, std::vector<unsigned int>> moduleDependencies;
  for(auto f: m_filters) {
    addDependencies(f,m_producers,moduleDependencies);
  }

  for(auto labelProd: m_producers) {
    addDependencies(labelProd.second,m_producers,moduleDependencies);
  }
  
  
  for(auto f: m_filters) {
    correctPossibleDeadlocks(f,m_producers,moduleDependencies);
  }
  m_filters.clear();

  for(auto const& labelProd: m_producers) {
    correctPossibleDeadlocks(labelProd.second,m_producers,moduleDependencies);
  }
  m_producers.clear();
}

void EventProcessor::handleNextEventAsync(WaitingTaskHolder iHolder,
                                          unsigned int iEventIndex) {
  Schedule& schedule = *(m_schedules[iEventIndex]);
  schedule.event()->reset();

  if(m_source->setEventInfo(*(schedule.event()))) {
    auto& group = iHolder.group();
    auto recursiveTask = make_waiting_task(
                          [this, h = std::move(iHolder), iEventIndex](std::exception_ptr const* iPtr) mutable {
        if(iPtr) {
          h.doneWaiting(*iPtr);
        } else {
          handleNextEventAsync(std::move(h), iEventIndex);
        }
      });
    schedule.processAsync(WaitingTaskHolder{group, recursiveTask});
  }
}



void EventProcessor::processAll(unsigned int iNumConcurrentEvents) {
  m_schedules.reserve(iNumConcurrentEvents);

  std::atomic<bool> done{false};
  std::exception_ptr exceptPtr;
  auto lastTask = make_waiting_task([&done, &exceptPtr](std::exception_ptr *iPtr) {
      if(iPtr) {
	exceptPtr = *iPtr;
      }
      done.store(true);
    });

  tbb::task_group waitGroup;
  {
    WaitingTaskHolder h{ waitGroup, lastTask};
    for(unsigned int nEvents = 1; nEvents<iNumConcurrentEvents; ++ nEvents) {
      Schedule* scheduleTemp = m_schedules[0]->clone();
      m_schedules.emplace_back(scheduleTemp);
      handleNextEventAsync(h, nEvents);
    }
    handleNextEventAsync(h,0);
  }

  do {
     waitGroup.wait();
  }while(not done.load());

  if(exceptPtr) {
    std::rethrow_exception(exceptPtr);
  }
}
