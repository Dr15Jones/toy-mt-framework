// Using gcc, we never see task stealing happen on the thread that 
//  started the taskloop
//g++ -std=c++17 -O3 omp_unsafe.cc -fopenmp -o omp_unsafe
//
// Using clang, we do see task stealing.
//clang++ -std=c++17 -O3 omp_unsafe.cc -fopenmp -o omp_unsafe
// show effect of using parallel for
//clang++ -std=c++17 -DPAR_FOR -O3 omp_unsafe.cc -fopenmp -o omp_unsafe_par_for
#include "omp.h"
#include <cmath>
#include <atomic>
#include <mutex>
#include <iostream>
#include <sys/time.h>

namespace demo {
  template<typename T>
    void parallel_for(unsigned int iRanges, T&& iFunctor) {
#if defined(PAR_FOR)
    omp_set_nested(1);
    omp_set_dynamic(1);
#pragma omp parallel for
#else
#pragma omp taskloop
#endif
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

  double integrate(int nIterations) {
    double v = 0;
    for(int k=0; k<nIterations; ++k) {
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
  void createOtherTask(int index, unsigned int nIterations, std::atomic<int>& waitingExtraTasks, std::atomic<int> const& threadIDUsedByParallel, std::atomic<int>& count) {
    ++waitingExtraTasks;
#pragma omp task untied default(shared) firstprivate(index,nIterations)
    {
      auto tid = threadID();
      {
        std::unique_lock<std::mutex> l{s_coutMutex};
        std::cout <<"Task "<<index<<" started on thread "<<tid<<std::endl;
      }
      --waitingExtraTasks;
      if(threadIDUsedByParallel == tid) {
        std::unique_lock<std::mutex> l{s_coutMutex};
        std::cout <<"REUSING SAME THREAD"<<std::endl;
      }
      task_on_thread_running(true);
      if(integrate(nIterations)>0.5) {
        ++count;
      }
      task_on_thread_running(false);
      std::unique_lock<std::mutex> l{s_coutMutex};
      std::cout <<"Task "<<index<<" ended on thread "<<tid<<std::endl;
    }
  }
}

int main() {

  omp_set_num_threads(10);
#if defined(PAR_FOR)
  std::cout <<"control the number of allowed threads with env variable OMP_THREAD_LIMIT"<<std::endl;
#endif

  std::atomic<int> count{0};


  std::atomic<int> countExtraWorkDone{0};
  std::atomic<bool> shouldStart{false};
  std::atomic<bool> startParallel{false};
  std::atomic<int> waitingExtraTasks{0};

  //Only needed for PAR_FOR case
  bool ranFirstLoop = false;

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
                                &shouldStart,&startParallel, &waitingExtraTasks, &ranFirstLoop](auto j) {
            auto tid = threadID();
            if(taskThreadID == tid) {
              std::unique_lock<std::mutex> l{s_coutMutex};
              std::cout <<"start parallel_for task "<<j<<" on task's thread "<<tid<<std::endl;
            } else {
              std::unique_lock<std::mutex> l{s_coutMutex};
              std::cout <<"start parallel_for task "<<j<<" on other thread "<<tid<<std::endl;
            }
            ++nStarted;
#if !defined(PAR_FOR)
            while(nStarted.load() <9) {}
#endif
            shouldStart = true;
            while(not startParallel) {};
            if(taskThreadID == tid) {
              {
                std::unique_lock<std::mutex> l{s_coutMutex};
                std::cout <<"start parallel_for work "<<j<<" on task's thread "<<tid<<std::endl;
              }
              if(not ranFirstLoop) {
                if(integrate(nIterations/10)>0.5) {
                  ++count;
                }
#if defined(PAR_FOR)
                ranFirstLoop = true;
              } else {
                if(integrate(100*nIterations)>0.5) {
                  ++count;
                }
#endif
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


  struct timeval tp;
  gettimeofday(&tp, 0);
  
  constexpr double microsecToSec = 1E-6;

  double realTime = tp.tv_sec - startRealTime.tv_sec + microsecToSec * (tp.tv_usec - startRealTime.tv_usec);

  std::cout <<"pass threshold "<<count.load()<<" max concurrent toplevel tasks "<<maxRunningTasks.load() <<" # times extra threads used "<<countExtraWorkDone.load()<<" time "<<realTime<<std::endl;
  return 0;
}
