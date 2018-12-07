//
//  parallel_for.h
//  OpenMPProcessingDemo
//
//  Created by Chris Jones on 10/4/18.
//  Copyright 2018 FNAL. All rights reserved.
//
#if !defined(OpenMPProcessingDemo_parallel_for_h)
#define OpenMPProcessingDemo_parallel_for_h

namespace demo {
  template<typename T>
    void parallel_for(unsigned int iRanges, T&& iFunctor) {
#pragma omp parallel for
    for(unsigned int i=0; i<iRanges; ++i) {
      iFunctor(i);
    }
  }

  template<typename T>
    void task_stealing_parallel_for(unsigned int iRanges, T&& iFunctor) {
#pragma omp taskloop
    for(unsigned int i=0; i<iRanges; ++i) {
      iFunctor(i);
    }
  }
}
#endif
