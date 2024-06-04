#pragma once
#include <iostream>

class Any {
 public:
  Any() = default;
  ~Any() = default;

  template <typename T>
  Any(const T& other) : storage_(new Value(other)) {}

  template <typename T>
  Any& operator=(const T& other) {
    delete storage_;
    storage_ = new Value<T>(other);

    return *this;
  }

  template <typename T>
  Any& operator=(const Any& other) {
    if (this != &other) {
      delete storage_;

      storage_ = other.storage_->Clone();
    }

    return *this;
  }

  template <typename T>
  static T any_cast(const Any& value) {
    if (value.storage_->GetTypeId() != MetaTypeInfo<T>::GetTypeInfo()) {
      std::cerr << "Bad cast";
      exit(1);
    }

    return static_cast<T>(static_cast<Value<T>*>(value.storage_)->value_);
  }

  template<typename T>
  operator T() const {
    return any_cast<T>(*this);
  }

  friend std::ostream& operator<<(std::ostream& stream, const Any& value);

 private:

  template <typename T>
  struct MetaTypeInfo {
    static int GetTypeInfo() {
      static const int typeID = Any::GenerateID();
      return typeID;
    }
  };

  static int GenerateID() {
    static int typeID = 0;
    return ++typeID;
  }

  struct ValueBase {
    virtual ~ValueBase() {}
    virtual void print(std::ostream& stream) = 0;
    virtual ValueBase* Clone() = 0;
    virtual int GetTypeId() const = 0;
  };

  template <typename T>
  struct Value : ValueBase {
    using value_type = T;

    Value(const T& val) : value_(val) {}

    int GetTypeId() const override {
      return MetaTypeInfo<T>::GetTypeInfo();
    }

    void print(std::ostream& stream) override {
      stream << this->value_;
    }

    ValueBase* Clone() override {
      return new Value<T>(value_);
    }

    T value_;
  };

  ValueBase* storage_;
};

std::ostream& operator<<(std::ostream& stream, const Any& value) {
  value.storage_->print(stream);
  return stream;
}