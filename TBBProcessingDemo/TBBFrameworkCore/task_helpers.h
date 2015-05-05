#ifndef Subsystem_Package_task_helpers_h
#define Subsystem_Package_task_helpers_h
// -*- C++ -*-
//
// Package:     Subsystem/Package
// Class  :     task_helpers
// 
/**\class task_helpers task_helpers.h "task_helpers.h"

 Description: functions which convert a C++ lambda into a TBB task

 Usage:
    <usage>

*/
//
// Original Author:  Christopher Jones
//         Created:  Tue, 05 May 2015 14:27:16 GMT
//

// system include files

// user include files

// forward declarations

namespace demo {

  namespace impl {
    template<typename T>
    class LambdaTask : public tbb::task {
    public:
      LambdaTask( const T& iAction) :
      m_action(iAction) {}
      
    private:
      tbb::task* execute() override {
        m_action();
        return nullptr;
      }

      T m_action;
    };
  }

  template <typename Alloc, typename T>
  tbb::task* make_task_from( Alloc allocation, const T& iAction ) {
    return new(allocation) impl::LambdaTask<T>(iAction);
  }

  template <typename T>
  void spawn_task_from(const T& iAction ) {
    tbb::task::spawn( *make_task_from(tbb::task::allocate_root(), iAction) );
  }

}

#endif
