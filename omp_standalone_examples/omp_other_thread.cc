// Using gcc 8.2 the taskyield does not cause any task to be stolen and
//  run on the thread
//g++ -std=c++17 -O3 omp_other_thread.cc -fopenmp -o omp_other_thread
//
// Using clang 6.01 the taskyield does do task stealing
//clang++ -std=c++17 -O3 omp_other_thread.cc -fopenmp -o omp_other_thread
#include "omp.h"
#include <cmath>
#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>

namespace {
  std::atomic<unsigned int> threadIDCount{0};
  
  std::mutex s_coutMutex;

  unsigned int threadID() {
    static thread_local const unsigned int s_id{ ++threadIDCount };
    return s_id;
  }

  template<typename T>
  void safeCout( T&& iF) {
    std::unique_lock<std::mutex> l(s_coutMutex);
    iF(std::cout);
  }

  double integrate(int nIterations) {
    double v = 0;
    for(int k=0; k<nIterations; ++k) {
      v += std::sin(3.14159/2.*k/nIterations)/double(nIterations);
    }
    return v;
  }

}

constexpr int nIterations = 1000000;

int main() {

  omp_set_num_threads(2);

  std::atomic<bool> startServer{false};
  std::atomic<bool> serverFinished{false};
  std::atomic<int> nCalls{0};

  std::thread serverThread([&startServer,&nCalls,&serverFinished]() {
      safeCout([](std::ostream& out) {
          out << "server thread id "<<threadID()<<std::endl;
        });
      while(not startServer.load()) ;
      if( 0.5 < integrate(2*nIterations)) {
        ++nCalls;
      }
      safeCout([](auto& out) {
          out << "server finished thread id "<<threadID()<<std::endl;
        });
      serverFinished = true;
    });

  std::atomic<int> nStartedTasksFromLoop{0};
#pragma omp parallel
  {
#pragma omp single
    {
#pragma omp task untied
      {
        safeCout([](auto& out) {
            out << "begin server start task thread id "<<threadID()<<std::endl;
          });
        while(nStartedTasksFromLoop.load() < 1);
        startServer=true;
        safeCout([](auto& out) {
            out << "begin taskyield thread id "<<threadID()<<std::endl;
          });
        //taskyield requires the result from the server to be processed by
        // the same thread that made the original request, even if there
        // is another OpenMP thread which is not busy.
        while(not serverFinished.load() ) {
#pragma omp taskyield
        }
        safeCout([](auto& out) {
            out << "taskyield done thread id "<<threadID()<<std::endl;
          });
#pragma omp task untied default(shared)
        {
          safeCout([](auto& out) {
              out << "begin task thread id "<<threadID()<<std::endl;
            });
          if( 0.5 < integrate(nIterations/10)) {
            ++nCalls;
          }
          safeCout([](auto& out) {
              out << "end task thread id "<<threadID()<<std::endl;
            });
          serverFinished = true;
        }

      }
      for(int i =0; i<4; ++i) 
#pragma omp task untied firstprivate(i) default(shared)
        {
          safeCout([i](auto& out) {
              out << "begin task index "<<i<<" thread id "<<threadID()<<std::endl;
            });
          ++nStartedTasksFromLoop;
          if( 0.5 < integrate(nIterations)) {
            ++nCalls;
          }
          safeCout([i](auto& out) {
              out << "end task index "<<i<<" thread id "<<threadID()<<std::endl;
            });
        }
    }
  }
  serverThread.join();
  std::cout <<"nCalls "<<nCalls.load()<<std::endl;

  return 0;
}
