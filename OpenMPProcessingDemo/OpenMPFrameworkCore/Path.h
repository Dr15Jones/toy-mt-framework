//
//  Path.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_Path_h
#define DispatchProcessingDemo_Path_h
#include <vector>
#include <boost/shared_ptr.hpp>


namespace demo {
  class Event;
  class Filter;
  class FilterWrapper;
  
  class Path {
  public:
    Path(): m_fatalJobErrorOccurredPtr(0) {}
    
    void run(Event&); 
    
    void reset();
    
    void setFatalJobErrorOccurredPointer(bool* iPtr) {
      m_fatalJobErrorOccurredPtr = iPtr;
    }
    
    void addFilter(FilterWrapper* iFilter);
    
    Path* clone(const std::vector<boost::shared_ptr<FilterWrapper> >& iWrappers) const;
    
  private:
    Path(const Path& iOther) = delete;

    std::vector<FilterWrapper*> m_filters;
    bool* m_fatalJobErrorOccurredPtr;
  };
  
}

#endif
