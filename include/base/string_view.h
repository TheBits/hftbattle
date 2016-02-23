#pragma once
#include <string>

namespace hftbattle {
class StringView {
public:
  typedef const char* iterator;

  constexpr StringView() : data_(nullptr), length_(0) { }

  constexpr StringView(const StringView&) = default;

  StringView(const char* begin, const char* end) :
      data_(begin), length_(static_cast<size_t>(end - begin)) { }

  StringView(const char* data, size_t length) : data_(data), length_(length) { }

  constexpr StringView(const char* data) : data_(data), length_(__builtin_strlen(data)) { }

  explicit StringView(const std::string& data) : data_(data.c_str()), length_(data.length()) { }

  template<size_t N>
  StringView(const char(&)[N]) {
    static_assert(
        N == N + 1,
        "StringView(const char[N]) is ambiguous. "
            "Use StringView(const char*) or StringView::from_char_array(const char[N] instead)"
    );
  }

  template<size_t N>
  static StringView from_char_array(const char(& data)[N]) {
    return StringView(data, N);
  }

  explicit operator bool() const { return length_ > 0; }

  constexpr char operator[](size_t index) const {
    return data_[index];
  }

  const char* data() const {
    return data_;
  }

  constexpr size_t length() const {
    return length_;
  }

  constexpr bool empty() const {
    return length_ == 0;
  }

  constexpr char front() const {
    return data_[0];
  }

  constexpr char back() const {
    return data_[length() - 1];
  }

  std::string std_str() const {
    return {begin(), end()};
  }

  StringView substr(size_t pos) const;
  StringView substr(size_t pos, size_t len) const;

  iterator begin() const { return data_; }

  iterator end() const { return data_ + length_; }

  size_t find(char c, size_t pos = 0) const;

  friend bool operator==(const StringView& lhs, const char* rhs);
  friend bool operator==(const StringView& lhs, const StringView& rhs);

private:
  const char* data_;
  size_t length_;
};

bool operator==(const StringView& lhs, const char* rhs);
bool operator==(const char* lhs, const StringView& rhs);

bool operator==(const StringView& lhs, const StringView& rhs);

bool operator==(const StringView& lhs, const std::string& rhs);
bool operator==(const std::string& lhs, const StringView& rhs);

bool operator!=(const StringView& lhs, const char* rhs);

bool operator!=(const StringView& lhs, const StringView& rhs);

bool operator!=(const StringView& lhs, const std::string& rhs);
bool operator!=(const std::string& lhs, const StringView& rhs);

std::string operator+(const std::string& lhs, const StringView& rhs);
std::string operator+(const StringView& lhs, const std::string& rhs);
std::string operator+(const char* lhs, const StringView& rhs);
std::string operator+(const StringView& lhs, const char* rhs);

}  // namespace hftbattle

