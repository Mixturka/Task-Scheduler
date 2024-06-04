#pragma once
#include <iostream>


template <typename Signature>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
 public:
  using result_type = R;

  static_assert(sizeof...(Args) <= 2, "Number of arguments must be less than or equal to 2.");
  template <typename Func>
  constexpr Function(const Func& func) : func_(new FuncObj<Func>(func)) {}
  

  result_type operator()(Args... args) {
    return func_->call(args...);
  }
    
 private:
  struct FuncBase {
    virtual result_type call(Args... args) = 0;

    virtual ~FuncBase() = default;
  };

  template <typename Func>
  struct FuncObj : FuncBase {
    FuncObj(const Func& func) : func_(func) {}

    result_type call(Args... args) override {
      return func_(args...);
    }

    Func func_;
  };

  FuncBase* func_;
};

