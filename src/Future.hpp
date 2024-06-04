#include "Any.hpp"

class Future {
 public:
  Future() = default;

  template <typename U>
  Future(const Future& other) {
    value = other.GetValue();
  }

  Future(Any& value, int& id) : value(&value), task_id(id) {}

  Any GetValue() const {
    return *value;
  }

  template <typename T>
  operator T() const {
    return *value;
  }

  friend std::ostream& operator<<(std::ostream& os, const Future& future) {
    os << *future.value;
    return os;
  }

 public:
  Any* value;
  int task_id;
};