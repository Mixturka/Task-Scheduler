# Task-Scheduler

## Overview
Task Scheduler designed for execution of data-related tasks. It creates a computation oriented graph where each nodes represents a task. Useful for optimizing computations that would otherwise be redundantly calculated, such as mathematics equations.

## Interface
- ```add```: Adds task to sheduler, returns pointer to TaskBase class
- ```getFutureResult```: Returns future task object from which the result can be obtained later
- ```getResult```: Returns result of task after executing all
- ```executeAll```: Executes all scheduled tasks

## Example usage
```cpp
  Function<float(float, float)> f([](float a, float b) { return a * b; });

  TTaskScheduler scheduler;
  float a = 1.f;
  float b = -2.f;
  float c = 0;
  
  auto id1 = scheduler.add([](float a, float c){return -4 * a * c;}, a, c);

  auto id2 = scheduler.add([](float b, float v){return b * b + v;}, b, scheduler.getFutureResult<float>(id1));

  auto id3 = scheduler.add([](float b, float d){return -b + std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

  auto id4 = scheduler.add([](float b, float d){return -b - std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

  auto id5 = scheduler.add([](float a, float v){return v/(2*a);}, a, scheduler.getFutureResult<float>(id3));

  auto id6 = scheduler.add([](float a, float v){return v/(2*a);},a, scheduler.getFutureResult<float>(id4));

  scheduler.executeAll();
  auto r = scheduler.getResult<float>(id5);
  std::cout << r << '\n';
  std::cout << scheduler.getResult<float>(id6);
```

## Implementation details
- Self implemented ```Any``` type erasure data type
- Self implemented ```Function``` type for comfortable function handling
