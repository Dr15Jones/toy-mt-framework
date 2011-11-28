/*
 *  GroupHolder.h
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/18/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef DispatchProcessingDemo_GroupHolder_h
#define DispatchProcessingDemo_GroupHolder_h

#include <dispatch/dispatch.h>
namespace demo {
   class GroupHolder {
   public:
      ///Pass newly created group to this constructor
      /// does not retain since dispatch_create already has count of 1
      explicit GroupHolder(dispatch_group_t iGroup):
      m_group(iGroup) {
      }
     enum RetainType {
       kDoRetain
     };
     GroupHolder(dispatch_group_t iGroup, RetainType):
     m_group(iGroup) {
       dispatch_retain(m_group);
     }
      ~GroupHolder() {
         dispatch_release(m_group);
      }
      
      GroupHolder(const GroupHolder& iHolder):
      m_group(iHolder.m_group) {
         dispatch_retain(m_group);
      }
      GroupHolder& operator=(const GroupHolder& iHolder) {
         GroupHolder t(iHolder);
         t.swap(*this);
         return *this;
      }
      void swap(GroupHolder& iHolder) {
         dispatch_group_t t = m_group;
         m_group = iHolder.m_group;
      iHolder.m_group = t;
      }
      
      dispatch_group_t get() const {
         return m_group;
      }
   private:
      dispatch_group_t m_group;
   };
}

#endif