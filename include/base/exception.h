#pragma once
#include "base/string_stream.h"
#include <exception>

namespace hftbattle {

class Exception: public std::exception {
  public:
    Exception() = default;
    Exception& operator=(const Exception&) = default;
    Exception(const Exception&) = default;

    const char* what() const noexcept {
      return what_.c_str();
    }

    template <typename T>
    Exception& operator <<(const T& t) {
      StringStream ostream;
      ostream << t;
      what_ += ostream.std_str();
      return *this;
    }
  private:
    std::string what_;
};

}  // namespace hftbattle
