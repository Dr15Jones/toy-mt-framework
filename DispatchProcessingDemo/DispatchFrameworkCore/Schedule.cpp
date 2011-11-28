//
//  Schedule.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#import <dispatch/dispatch.h>
#import <Block.h>
#include <assert.h>

#include "Schedule.h"
#include "Path.h"
#include "FilterWrapper.h"

using namespace demo;


Schedule::Schedule()
  : m_event(),
  m_allPathsDoneGroup(dispatch_group_create()),
  m_fatalJobErrorOccuredPtr(0){
}

Schedule::Schedule(Event* iEvent):
m_event(*iEvent),
m_allPathsDoneGroup(dispatch_group_create()),
m_fatalJobErrorOccuredPtr(0)
{  
}

Schedule::Schedule(const Schedule& iOther):
m_event(iOther.m_event),
m_allPathsDoneGroup(dispatch_group_create()),
m_fatalJobErrorOccuredPtr(iOther.m_fatalJobErrorOccuredPtr)
{
  m_filters.reserve(iOther.m_filters.size());
  for(FilterWrapper* fw: iOther.m_filters) {
    m_filters.push_back(new FilterWrapper(*fw,event()));
  }
  
  
  for(std::vector<Path*>::const_iterator it = iOther.m_paths.begin(), itEnd = iOther.m_paths.end();
      it != itEnd;
      ++it) {
    addPath((*it)->clone(m_filters));
  }
}


void 
Schedule::process(filtering_callback_t iCallback) {
  //printf("Schedule::process\n");
  reset();
  if(!m_paths.empty()) {
    //need to make a copy since iCallback can be on the stack
    filtering_callback_t heapCallback = Block_copy(iCallback);
    
    //items in a block become 'const' but we need non-const access to the event, hence use the pointer
    
    for(unsigned int index=0; index<m_paths.size();++index) {
      dispatch_group_async(m_allPathsDoneGroup.get(),dispatch_get_global_queue(0, 0), ^{
        processPresentPath(index);
      });
    }
    dispatch_group_notify(m_allPathsDoneGroup.get(), dispatch_get_global_queue(0, 0),^{
      heapCallback(not *m_fatalJobErrorOccuredPtr);
      Block_release(heapCallback);
    });
  } else {
    iCallback(true);
  }
}

void 
Schedule::addPath(Path* iPath) {
  m_paths.push_back(iPath);
  iPath->setFatalJobErrorOccurredPointer(m_fatalJobErrorOccuredPtr);
}

void
Schedule::addPath(const std::vector<std::string>& iPath) {
  std::auto_ptr<Path> newPath(new Path);
  for(const std::string& name: iPath) {
    FilterWrapper* fw = findFilter(name);
    if(0!=fw) {
      newPath->addFilter(fw);
    } else {
      assert(0=="Failed to find filter name");
      exit(1);
    }
  }
  addPath(newPath.release());
}

void
Schedule::addFilter(Filter* iFilter) {
  m_filters.push_back(new FilterWrapper(iFilter,&m_event));
}

void 
Schedule::reset() {
  dispatch_apply(m_paths.size(), dispatch_get_global_queue(0, 0), ^(size_t iIndex){
    m_paths[iIndex]->reset();
  });
}

Event*
Schedule::event()
{
  return &m_event;
}

FilterWrapper*
Schedule::findFilter(const std::string& iLabel){
  for(FilterWrapper* fw: m_filters) {
    if (fw->label() == iLabel) {
      return fw;
    }
  }
  return 0;
}

Schedule* 
Schedule::clone() {
  return new Schedule(*this);
}

//NOTE: Must pass a heap based callback to this code
void 
Schedule::processPresentPath(unsigned int iIndex) {
  //printf("Schedule::processPresentPath %u\n",iIndex);      
  if(*m_fatalJobErrorOccuredPtr) {
    return;
  }
  //Enter the group here and leave once the path has finished
  dispatch_group_enter(m_allPathsDoneGroup.get());
  
  dispatch_group_t groupPtr = m_allPathsDoneGroup.get();
  m_paths[iIndex]->runAsync(m_event, ^(bool iSuccess){
    if(!iSuccess) {
      *(this->m_fatalJobErrorOccuredPtr) = true;
    }
    dispatch_group_leave(groupPtr);
  });
}
