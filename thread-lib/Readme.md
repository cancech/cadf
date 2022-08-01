# thread-lib: Threading Library

Builds on top of the Standard C++ thread capabilities to provide some additional quality of life add-ons.

## cadf :: thread :: ITask

A unit of work that is to be executed within a thread. `cadf::thread::Task` is an abstract class where the following methods must be implemented:

* `exec()` called within the thread to execute the task. Any work the task is to perform within the thread should be done here.

* `scheduleStop()` called by the thread as part of the termination process for the task, to make the task aware that it should stop its execution. For example, to flip a condition that a loop is using to determine whether to continue looping.

The [Task](include/thread/Task.h) provides helper methods `ssleep()`, `msleep()`, and `usleep()` which will sleep the thread for the specified `unsigned int` number of seconds, milliseconds, and microseconds respectively.

One [Task](include/thread/Task.h) implementation is provided in the form of `cadf::thread::LoopingTask`, which provides the necessary logic to support a task that will continue executing/looping until the thread is terminated (`scheduleStop()` is called). To make use of it, simple extend and override the `execLoop()` method. The `exec()` in the parent [LoopingTask](include/thread/Task.h) will take care of the repeatedly calling `execLoop()` with each iteration of the loop, as well as breaking the loop when `scheduleStop()` is called.

```C++
class MyLoopingTask: public cadf::thread::LoopingTask {
  public:
    void execLoop() {
      std::cerr << "I'm still alive!" << std::endl;
      ssleep(1);
    }
};
```
The above example task will print _I'm still alive!_ once per second until the thread in which is it executing is terminated.

## cadf :: thread :: Thread

Largely just a wrapper for `std::thread`, it performs thread management for the provided [Task](include/thread/Task.h) it is executing.

```
cadf::thread::Thread myThread(myTask);
myThread.start();
```

The thread will stay alive until the specified task completes. There is a mechanism that will attempt to terminate the thread prior to the completion of the task.

```
myThread.stop();
```

This will first call `cadf::thread::Task::scheduleStop()` to notify the task that an end is coming (i.e.: an alternative condition for ending a loop), and send an interrupt signal `SIGNINT` to interrupt any blocking behavior. The expectation is that this combination should be sufficient to allow for a clean termination of a thread. This will not be sufficient to break out everything a task could be performing (i.e.: `while(true);` will not be stopped), so care must be taken when designing tasks to account for this.

To facilitate ease of use, two [Thread](include/thread/Thread.h) subclasses are provided: `cadf::thread::OneShotThread` and `cadf::thread::LoopingThread`. These can be extended to allow the creation of thread classes that are their own task. As the names imply, [OneShotThread](include/thread/Thread.h) will perform a single execution of the `exec()` method when started, while [LoopingThread](include/thread/Thread.h) will continuously execute `execLoop()` until the thread is stopped (as per [LoopingTask](include/thread/Task.h)).

## cadf :: thread :: BasicThreadPool

An implementation of the pure virtual [cadf :: thread :: IThreadPool](include/thread/ThreadPool.h) iterface, it provides the capability to schedule small pieces of work that are to be executed asynchronously, without excessive overhead of creating, managing, and destroying threads. The number of threads that [BasicThreadPool](include/thread/BasicThreadPool.h) will allow for is specified in the constructor (defaults to `std::thread::hardware_concurrency()`), as well as whether or not it should start automatically.

```
cadf::thread::BasicThreadPool noAutoStart(42, false);
cadf::thread::BasicThreadPool defaultAutoStart;
```

Work that is to be performed within the thread pool must be scheduled, and it will be executed on a first come, first serve basis. If all threads are busy, then any additional scheduled work will be queued until a thread in the pool become available to execute it. For maximum usability any `std::function<void()>` can be sheduled.

```
cadf::thread::BasicThreadPool myPool;
myPool.schedule(std::bind(&MyClass::myMethod, myInstance));
```

For more complex tasks a separate [Thread](include/thread/Thread.h) should be created
