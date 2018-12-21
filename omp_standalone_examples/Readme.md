# Introduction
I have  been exploring how a High Energy Physics (HEP) data processing framework could make use of OpenMP as its concurrency engine. This turned up several challenges which are illustrated by the simple programs in this directory.


#HEP Data Processing
HEP data is composed of a series of statistically independent Records. The statistical independency allows those Records to be processed in any order and therefore is perfectly parallelizable. To process a single Record requires a large number of algorithms to be run. Some algorithms depend on the results of other algorithms which forms a directed graph of algorithmic dependencies. A second level of parallelization can be achieved by running non-dependent algorithms concurrently. Finally, some of the individual algorithms themselves may have internal parts that are amenable to being run concurrently. We wish to exploit all three levels of concurrency within our application.

There are cases where a given algorithm may not be able to run concurrently using different Records. One such case is when the algorithm is writing data to a file. In such cases we avoid the use of locks by using an algorithm scheduling mechanism which only runs one instance of such an algorithm at a time even if multiple instances are waiting to be run. This frees up the scheduler to run other algorithms which can make progress.

One constraint we have is we run on computing hardware which is shared with others. Therefore our jobs are required to not use more CPU cores than a value which is specified at run time.

All levels of concurrency are implemented making use of the task mechanics in TBB or OpenMP. For each new Record to be processed, we begin requesting data needed for the algorithms we know for certain we must run (e.g. the algorithm which stores the new results). Some of this requested data may come from other algorithms which causes their data to also be requested. The data requests continue until we reach an algorithm that takes no input or we need to read the requested data from an input file. In either case, we wrap the algorithm or the read into a task and schedule the task to be run. Once all data for a given algorithm being run on a given Record are available, we create a new task to run the waiting algorithm. When running, if an algorithm can do its work in parallel, that work is broken into different tasks (possibly behind the scenes using a parallel_for function) and those tasks are scheduled to be run.  In this way, both the parallelizing of algorithms and parallelizing the internals of algorithms can make use of the same task scheduling system and, hopefully, thread pool.

#Sub-algorithm concurrency
As mentioned previously, some algorithms internally support concurrency. One natural way to handle that is to parallelize a `for` loop. Program `omp_starved.cc` illustrates one challenge in using a parallel `for`. If one naively uses just `#pragma omp for` then the program will deadlock at the end of the `for` loop. It appears that the `for` loop is waiting for all outstanding tasks in the `parallel` section to complete, but the `for` is part of one of those tasks and therefore it appears to be waiting for itself to complete. Switching to `#pragma omp parallel for` does work, but does not give optimal performance. Using `#pragma omp taskloop` gives the optimal performance, though opens up the program to the problems inherent in task stealing.

#Difficulty with composability
The preceding section appears exposed an apparent difficulty that in order to call a component using OpenMP parallelization, one must know exact which OpenMP constructs were used, e.g. `#pragma omp for` version `#pragma parallel omp for`. Even with such knowledge, it can be difficult to optimally tune the performance of the application when one has a limited number of threads allowed for use.

#Problems with task stealing
In the cases where an algorithm internally is making use of parallelization, allowing task stealing on for the thread which must wait for all sub-parallelization tasks to finish can lead to problems. The problem stems from the case where the algorithm doing the parallelization is holding onto a shared resource. In that case, allowing task stealing can lead to performance degradations or even to deadlocks. The deadlock case is the easiest to explain. If the originating algorithm is holding shared resource A and then the thread running the originating algorithm steals a task which also wants resource A we can get a deadlock.

Program `omp_unsafe.cc` just shows that the gcc 8.2 compiler does not use task stealing with `taskloop` while the clang 7.0.0 compiler does do task stealing. This program can also be compiled with the option `-DPAR_FOR` to switch from `taskloop` to `parallel for` in order to avoid the task stealing. Using that option creates a program that is more complicated to configure (since one must specify the number of threads to use for each `parallel` section as well as the total number of threads the program is allowed to use) and is less performant.

The program `omp_bad_task_steal.cc` illustrates how task stealing can lead to degraded performance. For this program, the non-task stealing gcc version runs nearly 25% faster than the task stealing clang version. The output of the program (including the use of the linux `time` command) was

*clang*
```
 n Threads 20 # concurrent processing loops 10 # items to process 500
 # processed 500 times had multiple tasks on a thread 28 max concurrent toplevel tasks 21
 317.973u 4.615s 1:32.92 347.1%	0+0k 4992+0io 18pf+0w
```
*gcc*
```
 n Threads 20 # concurrent processing loops 10 # items to process 500
 # processed 500 times had multiple tasks on a thread 0 max concurrent toplevel tasks 20
 1.168u 0.085s 1:17.31 1.6%	0+0k 6776+0io 24pf+0w
```

#Interacting with non-OpenMP Threads

HEP data processing has a need to interact with asynchronous non CPU computational resources. These could be GPUs, FPGAs, neuromorphic chips or even other processes. While waiting for the results of such computations, we would like to have all the threads assigned to the job be processing other tasks. When the asynchronous computation finishes, we would like to be able to add a new task to the system to make use of the results. When using TBB, we do this by using TBB's _task enqueueing_ ability which allows a task to be created and put on a queue from a non-TBB thread. In this way our programs can have auxiliary threads which are waiting for the results of the asynchronous computation and when that computation finishes the auxiliary thread can enqueue a new TBB task and that task will eventually be run under a TBB controlled thread. As far as I have been able to find, there is no such facility for OpenMP. It appears that all OpenMP task sections must be processed by an existing OpenMP thread.

A possible way forward is to make use of the `taskyield` facility. In the task which launches the asynchronous computation, one could immediately after launch have a loop that checks for completion of the task and if it is not complete, one calls `taskyield` in order to allow another OpenMP task to make use of the thread. The program `omp_other_thread.cc` illustrates the use of `taskyield`. One observation in that program is the clang runtime will do task stealing in a `taskyield` while gcc will not.

A major downside to using the `taskyield` facility is the thread which launches the asynchronous computation must also be the thread which responds to the finishing of that computation. If the `taskyield` winds up stealing a very long running task, then the processing thread must finish that long running task before it can respond to the completion of the computation. The response must wait even if other OpenMP threads have no other tasks to process. In the TBB case, any available TBB thread is allowed to process the response.
