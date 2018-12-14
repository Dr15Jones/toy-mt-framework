// Using gcc, we never see task stealing happen on the thread that 
//  started the taskloop
//g++ -std=c++17 -O3 omp_bad_task_steal.cc -fopenmp -o omp_bad_task_steal
//
// Using clang, we do see task stealing.
//clang++ -std=c++17 -O3 omp_bad_task_steal.cc -fopenmp -o omp_bad_task_steal
//
// To run
//./omp_bad_task_steal 20 10 500 
#include "omp.h"
#include <cmath>
#include <atomic>
#include <mutex>
#include <iostream>
#include <memory>
#include <unistd.h>
//#include <sys/time.h>

//#define DEBUG_OUT

namespace demo {

  //A general interface usable by both TBB and OpenMP implementations
  template<typename T>
    void parallel_for(unsigned int iRanges, T iFunctor) {
#pragma omp taskloop
    for(unsigned int i=0; i<iRanges; ++i) {
      iFunctor(i);
    }
  }
}

namespace {
  
  std::mutex s_coutMutex;

  std::atomic<unsigned int> threadIDCount{0};
  unsigned int threadID() {
    static thread_local const unsigned int s_id{ ++threadIDCount };
    return s_id;
  }

  /*
    We want to keep track of how much concurrency the program is accomplishing
    and the case where we have task stealing occuring.
   */
  std::atomic<int> countRunningTasks{0};
  std::atomic<int> maxRunningTasks{0};
  std::atomic<int> nTimesMultipleTasksOnThread{0};

  /* iState == true means we are starting a task on this thread
     iState == false means we are stopping the task
   */
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
        ++nTimesMultipleTasksOnThread;
#if defined(DEBUG_OUT)
        std::unique_lock<std::mutex> l{s_coutMutex};
        std::cout <<"more than one task ("<<task_on_thread_count<<") on thread "<<threadID()<<std::endl;
#endif
      }
    } else {
      --task_on_thread_count;
      --countRunningTasks;
    }
  }


  // Keeps track of how processing is proceeding
  struct ProcessingState {
    std::atomic<int> m_leftToProcess;
    std::atomic<int> m_nProcessed;
  };

  // In the full program, only one thread can do work on the storage at a time
  class MockStorage {
  public:
    MockStorage(long int nIterations) : m_nIterations{nIterations}, m_inUse{false},m_nWaiting{0} {}

    void storeAsync(ProcessingState& iState);
  private:

    void doNext(ProcessingState& iState);
    void store(ProcessingState& iState);
    std::atomic<bool> m_inUse;  //True if a thread is running storage task
    std::atomic<int> m_nWaiting; //Number of processing loops that want storage to happen
    const long int m_nIterations;
  };

  void longProcessTask(ProcessingState& iState, MockStorage& iStore, long int nIterations, std::shared_ptr<std::atomic<short>> iPtr);

  void processNextAsync(ProcessingState& iState, MockStorage& iStore, long int nIterations) {
    auto l = --iState.m_leftToProcess;
    if( l >= 0 ) {
      {
#if defined(DEBUG_OUT)
        std::unique_lock<std::mutex> lck{s_coutMutex};
        std::cout <<l<<std::endl;
#endif
      }
      //Only once these 3 tasks complete can we run storage
      auto sp = std::make_shared<std::atomic<short>>(3);
#pragma omp task untied default(shared) firstprivate(nIterations, sp)
      {
        longProcessTask(iState,iStore, nIterations, sp);
      }
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

    usleep(nIterations);
    task_on_thread_running(false);
    if ( 0 == --(*iPtr) ) {
      //if this is the last task for this processing, queue up storage
      iStore.storeAsync(iState);
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
     auto tid = threadID();
    task_on_thread_running(true);

    std::atomic<int> nParallel{0};
    std::atomic<int> maxParallel{0};

    //There is some tuning going on to show the effect
    // storage has to run long enough to task steal but shorter than all other tasks
    // so the stolen task prolongs the storage time sufficiently to cause a problem.

    demo::parallel_for(5, [nIterations=m_nIterations, &nParallel, &maxParallel, tid](int iIndex) {
        auto p = ++nParallel;
        auto max =maxParallel.load();
        while(p > max) {
          maxParallel.compare_exchange_strong(max,p);
        }
        auto n = nIterations/5/20;
        if(tid != threadID()) {
          task_on_thread_running(true);
        }
        if(iIndex == 4) {
          n *= 3;
        }
        usleep(n);
        if(tid != threadID() ) {
          task_on_thread_running(false);
        }
        --nParallel;
      });

    if(maxParallel > 1) {
#if defined(DEBUG_OUT)
      std::unique_lock<std::mutex> l{s_coutMutex};
      std::cout <<"max parallel " <<maxParallel.load()<<std::endl;
#endif
    }

    task_on_thread_running(false);

    processNextAsync(iState, *this, m_nIterations);

    m_inUse = false;

    ++iState.m_nProcessed;
    //see if anything is waiting on us
    doNext(iState); 
  }
}

int main(int argc, char * argv[] ) {

  if(argc != 4) { return 1;}


  auto const nThreads = std::stoi(argv[1]);

  omp_set_num_threads(nThreads);

  const int nEvents = std::stoi(argv[3]);
  ProcessingState state = {nEvents, 0};
  constexpr long int nIterations = 1000000; // 1 second in usecs

  MockStorage storage(nIterations);

  const int nStreams = std::stoi(argv[2]);
#pragma omp parallel default(shared)
  {
#pragma omp single
    {
      for(unsigned int i=0; i< nStreams; ++i ) {
        processNextAsync(state, storage,nIterations);
      }
    }
  }

  std::cout <<"n Threads "<<nThreads<<" # concurrent processing loops "<<nStreams <<" # items to process "<< nEvents<<std::endl;
  std::cout <<"# processed "<<state.m_nProcessed.load() <<" times had multiple tasks on a thread "<< nTimesMultipleTasksOnThread << " max concurrent toplevel tasks "<<maxRunningTasks.load() <<std::endl;
  return 0;
}
