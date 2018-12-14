// Using gcc, we never see task stealing happen on the thread that 
//  started the taskloop
//g++ -std=c++17 -O3 omp_bad_task_steal.cc -fopenmp -o omp_bad_task_steal
//
// Using clang, we do see task stealing.
//clang++ -std=c++17 -O3 omp_bad_task_steal.cc -fopenmp -o omp_bad_task_steal
#include "omp.h"
#include <cmath>
#include <atomic>
#include <mutex>
#include <iostream>
#include <memory>
//#include <sys/time.h>

namespace demo {
  template<typename T>
    void parallel_for(unsigned int iRanges, T iFunctor) {
#pragma omp taskloop
    for(unsigned int i=0; i<iRanges; ++i) {
      iFunctor(i);
    }
  }
}

namespace {
  std::atomic<unsigned int> threadIDCount{0};
  
  std::mutex s_coutMutex;

  unsigned int threadID() {
    static thread_local const unsigned int s_id{ ++threadIDCount };
    return s_id;
  }

  double integrate(long int nIterations) {
    double v = 0.;
    for(long int k=0; k<nIterations; ++k) {
      v += std::sin(3.14159/2.*k/nIterations)/double(nIterations);
    }
    return v;
  }

  std::atomic<int> countRunningTasks{0};
  std::atomic<int> maxRunningTasks{0};
  void task_on_thread_running(bool iState) {

    static thread_local unsigned int task_on_thread_count{0};
    if(iState) {
      auto crt = ++countRunningTasks;
      auto maxRT = maxRunningTasks.load();
      while(crt > maxRT) {
        maxRunningTasks.compare_exchange_strong(maxRT,crt);
      }

      ++task_on_thread_count;
      if(task_on_thread_count > 1) {
        std::unique_lock<std::mutex> l{s_coutMutex};
        std::cout <<"more than one task ("<<task_on_thread_count<<") on thread "<<threadID()<<std::endl;
      }
    } else {
      --task_on_thread_count;
      --countRunningTasks;
    }
  }


  struct ProcessingState {
    std::atomic<int> m_leftToProcess;
    std::atomic<int> m_nProcessed;
  };

  class MockStorage {
  public:
    MockStorage(long int nIterations) : m_nIterations{nIterations}, m_inUse{false},m_nWaiting{0} {}

    void storeAsync(ProcessingState& iState);
  private:

    void doNext(ProcessingState& iState);
    void store(ProcessingState& iState);
    std::atomic<bool> m_inUse;
    std::atomic<int> m_nWaiting;
    const long int m_nIterations;
  };

  void longProcessTask(ProcessingState& iState, MockStorage& iStore, long int nIterations, std::shared_ptr<std::atomic<short>> iPtr);

  void processNextAsync(ProcessingState& iState, MockStorage& iStore, long int nIterations) {
    auto l = --iState.m_leftToProcess;
    if( l > 0 ) {
      std::cout <<l<<std::endl;
      auto sp = std::make_shared<std::atomic<short>>(2);
#pragma omp task untied default(shared) firstprivate(nIterations, sp)
      {
        longProcessTask(iState,iStore, nIterations, sp);
      }
#pragma omp task untied default(shared) firstprivate(nIterations, sp)
      {
        longProcessTask(iState,iStore, nIterations, sp);
      }
    }
  }

  void longProcessTask(ProcessingState& iState, MockStorage& iStore, long int nIterations, std::shared_ptr<std::atomic<short>> iPtr) {
    task_on_thread_running(true);

    if(integrate(nIterations) > 0.5) {
      //this is always true and just exists to avoid optimizing away the function call
      task_on_thread_running(false);
      if ( 0 == --(*iPtr) ) {
        iStore.storeAsync(iState);
      }
    }
  }

  void
  MockStorage::storeAsync(ProcessingState& iState) {
    ++m_nWaiting;
    doNext(iState);
  }

  void MockStorage::doNext(ProcessingState& iState) {
    while(m_nWaiting > 0) {
      bool expected = false;
      if ( m_inUse.compare_exchange_strong(expected, true) ) {
        if (m_nWaiting > 0) {
          --m_nWaiting;
#pragma omp task untied default(shared)
          {
            store(iState);
          }
        } else {
          m_inUse = false;
        }
      } else {
        //another thread has acquired the 'm_inUse' so that
        // thread is required to decrement the wait count
        break;
      }
    }
  }

  void MockStorage::store(ProcessingState& iState) {
    std::array<double, 5> values = {{0,0,0,0,0}};
    auto tid = threadID();
    task_on_thread_running(true);

    std::atomic<int> nParallel{0};
    std::atomic<int> maxParallel{0};
    demo::parallel_for(5, [&values, nIterations=m_nIterations, &nParallel, &maxParallel, tid](int iIndex) {
        auto p = ++nParallel;
        auto max =maxParallel.load();
        while(p > max) {
          maxParallel.compare_exchange_strong(max,p);
        }
        auto n = nIterations/5/10;
        if(tid != threadID()) {
          task_on_thread_running(true);
        } /*else {
          n = n/10;
          } */
        if(iIndex == 3) {
          n *= 5;
        }
        values[iIndex] = integrate(n);
        if(tid != threadID() ) {
          task_on_thread_running(false);
        }
        --nParallel;
      });

    //if(maxParallel > 1) {
    std::cout <<"max parallel " <<maxParallel.load()<<std::endl;
      //}
    double sum = 0;
    for( auto v : values) {
      sum += v;
    }
    task_on_thread_running(false);

    if(sum > 0.01) {
      processNextAsync(iState, *this, m_nIterations);
    }
    m_inUse = false;

    ++iState.m_nProcessed;
    //see if anything is waiting on us
    doNext(iState); 
  }
}

int main() {

  omp_set_num_threads(12);

  ProcessingState state = {100, 0};
  constexpr long int nIterations = 100000000;
  //constexpr long int nIterations = 100;
  MockStorage storage(nIterations);

  constexpr int nStreams = 12;
#pragma omp parallel default(shared)
  {
#pragma omp single
    {
      for(unsigned int i=0; i< nStreams; ++i ) {
        processNextAsync(state, storage,nIterations);
      }
    }
  }
  
  std::cout <<"n processed "<<state.m_nProcessed.load()<<" max concurrent toplevel tasks "<<maxRunningTasks.load() <<std::endl;
  return 0;
}
  /*
  std::atomic<int> count{0};


  std::atomic<int> countExtraWorkDone{0};
  std::atomic<bool> shouldStart{false};
  std::atomic<bool> startParallel{false};
  std::atomic<int> waitingExtraTasks{0};

  struct timeval startRealTime;
  gettimeofday(&startRealTime, 0);

  constexpr int nIterations = 10000000;

  std::atomic<int> threadIDUsedByParallel{-1};

#pragma omp parallel default(shared)
  {
#pragma omp single
    {
#pragma omp task untied default(shared)
      {
        while(not shouldStart.load());
        for(int i=0; i<10; ++i) {
          createOtherTask(i,nIterations,waitingExtraTasks,threadIDUsedByParallel,count);
        }
        startParallel = true;
      }

#pragma omp task untied default(shared)
      {
        task_on_thread_running(true);
        auto taskThreadID = threadID();
        threadIDUsedByParallel = taskThreadID;
        {
          std::unique_lock<std::mutex> l{s_coutMutex};
          std::cout <<"thread id used by parallel task starter "<<taskThreadID<<std::endl;
        }
        std::atomic<int> nStarted{0};
        demo::parallel_for(10, [&count,taskThreadID,&countExtraWorkDone,&nStarted,
                                &shouldStart,&startParallel, &waitingExtraTasks](auto j) {
            auto tid = threadID();
            if(taskThreadID == tid) {
              std::unique_lock<std::mutex> l{s_coutMutex};
              std::cout <<"start parallel_for task "<<j<<" on task's thread "<<tid<<std::endl;
            } else {
              std::unique_lock<std::mutex> l{s_coutMutex};
              std::cout <<"start parallel_for task "<<j<<" on other thread "<<tid<<std::endl;
            }
            ++nStarted;
            while(nStarted.load() <9) {}
            shouldStart = true;
            while(not startParallel) {};
            if(taskThreadID == tid) {
              {
                std::unique_lock<std::mutex> l{s_coutMutex};
                std::cout <<"start parallel_for work "<<j<<" on task's thread "<<tid<<std::endl;
              }
              if(integrate(nIterations/10)>0.5) {
                ++count;
              }
              std::unique_lock<std::mutex> l{s_coutMutex};
              std::cout <<"waiting extra tasks "<<waitingExtraTasks.load()<<std::endl;
              return;
            }
            {
              std::unique_lock<std::mutex> l{s_coutMutex};
              std::cout <<"start parallel_for work "<<j<<" on other thread "<<tid<<std::endl;
            }
            ++countExtraWorkDone;
            if(integrate(100*nIterations)>0.5) {
              ++count;
            }
            std::unique_lock<std::mutex> l{s_coutMutex};
            std::cout <<"end parallel_for work "<<j<<" on other thread "<<tid<<std::endl;

          });
        task_on_thread_running(false);
        threadIDUsedByParallel = -1;
      }
    }
  }

  */
/*
  struct timeval tp;
  gettimeofday(&tp, 0);
  
  constexpr double microsecToSec = 1E-6;

  double realTime = tp.tv_sec - startRealTime.tv_sec + microsecToSec * (tp.tv_usec - startRealTime.tv_usec);

  std::cout <<"pass threshold "<<count.load()<<" max concurrent toplevel tasks "<<maxRunningTasks.load() <<" # times extra threads used "<<countExtraWorkDone.load()<<" time "<<realTime<<std::endl;
  return 0;
}
*/
