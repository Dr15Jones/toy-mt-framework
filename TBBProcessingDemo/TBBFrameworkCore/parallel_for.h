//
//  parallel_for.h
//  TBBProcessingDemo
//
//  Created by Chris Jones on 10/4/18.
//  Copyright 2018 FNAL. All rights reserved.
//
#if !defined(TBBProcessingDemo_parallel_for_h)
#define TBBProcessingDemo_parallel_for_h
#include "tbb/task_arena.h"
#include "tbb/parallel_for.h"

namespace demo {
  template<typename T>
    void parallel_for(unsigned int iRanges, T&& iFunctor) {
    tbb::this_task_arena::isolate( [&] {
        tbb::parallel_for( 0U, iRanges, [&]( int j ) { iFunctor(j); });
      });
  }

  template<typename T>
    void task_stealing_parallel_for(unsigned int iRanges, T&& iFunctor) {
    tbb::parallel_for( 0U, iRanges, [&]( int j ) { iFunctor(j); });
  }
}
#endif
