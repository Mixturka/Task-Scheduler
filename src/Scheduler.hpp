#pragma once
#include <iostream>
#include <vector>
#include "Function.hpp"
#include "Any.hpp"
#include "Future.hpp"

template <size_t... Is>
struct IndexSequence {};

template <size_t N, size_t... Is>
struct MakeIndexSequence : MakeIndexSequence<N - 1, N - 1, Is...> {};

template <size_t... Is>
struct MakeIndexSequence<0, Is...> {
  using type = IndexSequence<Is...>;
};

template <size_t N>
using CreateIndexSeq = typename MakeIndexSequence<N>::type;

class TTaskScheduler {
 class TaskBase;
 public:
  TTaskScheduler() = default;

  template <typename Func, typename... Args>
  TaskBase* add(const Func& func, Args... args) {
    TaskBase* new_task = new Task<Func, Args...>(func, args...);
    new_task->task_id = next_task_id_++;
    task_graph_.emplace_back(std::vector<TaskBase*>());
    tasks_.push_back(new_task);
    future_results_.resize(future_results_.size() + 1);
    future_results_[new_task->task_id] = Future(static_cast<Task<Func, Args...>*>(new_task)->result, new_task->task_id);
    
    if (HasFuture(args...)) {
      GetFutureArgument(args...);

      for (Future& future : found_futures_) {
        task_graph_[tasks_[future.task_id]->task_id].push_back(new_task);
      }

      found_futures_.clear();
    }

    return new_task;
  }

  template <typename T>
  Future& getFutureResult(TaskBase* task) {
    return future_results_[task->task_id];
  }

  void executeAll() {
    is_executed_ = true;
    for (int i = 0; i < tasks_.size(); ++i) {
     tasks_[i]->Execute();
    }
  }

  template<typename T>
  T getResult(TaskBase* task) {
    if (is_executed_) {
      return *future_results_[task->task_id].value;
    }

    if (!is_executed_) {
      for (int i = 0; i < task->task_id + 1; ++i) {
        tasks_[i]->Execute();
      }

      return *future_results_[task->task_id].value;
    }
  }

 private:
  class TaskBase {
   public:
    int index_;
    virtual ~TaskBase() = default;
    virtual void Execute() = 0;
    virtual Any GetResult() = 0;
    std::vector<TaskBase*> dependencies_;
    int task_id;
  };

  template <typename Func, typename... Args>
  class Task : public TaskBase {
   public:
    Task(Func func, Args... args) : func_(func) {
        AddArgs(args...);
    }

    void Execute() override {
      CallFunc(CreateIndexSeq<sizeof...(Args)>{}); 
    }
    
    Any GetResult() override {
      return result;
    }

   public:
    template <typename Arg>
    void AddArgs(Arg arg) {
      arguments_.push_back(Any(arg));
    }

    void AddArgs() {}

    template <typename Arg, typename... Rest>
    void AddArgs(Arg arg, Rest... rest) {
      arguments_.push_back(Any(arg));
      AddArgs(rest...);
    }

    template <size_t... Is>
    void CallFunc(IndexSequence<Is...>) {
      result = func_(arguments_[Is]...);
    }

    void CallFunc(IndexSequence<>) {
      result = func_();
    }

    Function<Any(Args...)> func_;
    std::vector<Any> arguments_;
    Any result;
  };

  bool HasFuture() {
    return false;
  }

  template <typename T>
  struct IsFuture {
    static const bool value = false;
  };

  template <>
  struct IsFuture<Future> {
    static const bool value = true;
  };

  template <typename First, typename... Other>
  bool HasFuture(First first, Other... other) {
    return IsFuture<First>::value || HasFuture(other...);
  }

  void GetFutureArgument() {}

  template <typename... Other>
  void GetFutureArgument(Future& future, Other&... other) {
    found_futures_.push_back(future);
    GetFutureArgument(other...);
  }

  template <typename First, typename... Other>
  void GetFutureArgument(First&, Other&... other) {
    GetFutureArgument(other...);
  }

  std::vector<std::vector<TaskBase*>> task_graph_;
  std::vector<TaskBase*> tasks_;
  int next_task_id_ = 0;
  std::vector<Future> future_results_;
  std::vector<Future> found_futures_;
  bool is_executed_ = false;
};