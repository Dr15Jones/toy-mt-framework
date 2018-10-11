//g++ -std=c++17 -O3 omp_other_thread.cc -fopenmp -o omp_other_thread
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
  std::atomic<int> nCalls{0};

  std::thread serverThread([&startServer,&nCalls]() {
      safeCout([](std::ostream& out) {
          out << "server thread id "<<threadID()<<std::endl;
        });
      while(not startServer.load()) ;

#pragma omp task untied default(shared)
      {
        safeCout([](auto& out) {
            out << "begin task thread id "<<threadID()<<std::endl;
          });
        if( 0.5 < integrate(nIterations)) {
          ++nCalls;
        }
        safeCout([](auto& out) {
            out << "end task thread id "<<threadID()<<std::endl;
          });
      }
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
            out << "end server start task thread id "<<threadID()<<std::endl;
          });
      }
#pragma omp taskloop
      for(int i =0; i<3; ++i) 
        {
          safeCout([i](auto& out) {
              out << "begin taskloop index "<<i<<" thread id "<<threadID()<<std::endl;
            });
          ++nStartedTasksFromLoop;
          if( 0.5 < integrate(nIterations)) {
            ++nCalls;
          }
          safeCout([i](auto& out) {
              out << "end taskloop index "<<i<<" thread id "<<threadID()<<std::endl;
            });
        }
    }
  }
  serverThread.join();
  std::cout <<"nCalls "<<nCalls.load()<<std::endl;

  return 0;
}
