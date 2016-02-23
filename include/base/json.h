#pragma once
#include "base/decimal.h"
#include "base/string_view.h"
#include "base/log.h"
#include <string>
#include <vector>
#include <memory>

namespace Json {
class ValueIterator;
class Value;
}

namespace hftbattle {

enum class JsonValueType {
  Null,
  Int,
  Real,
  Bool,
  String,
  Array,
  Object
};

class JsonValue;
class JsonValueIterator {
public:
  typedef JsonValue& reference;
  explicit JsonValueIterator(const Json::ValueIterator&);
  JsonValueIterator(const JsonValueIterator&);
  ~JsonValueIterator();
  JsonValueIterator& operator++();
  JsonValue operator*() const;
  bool operator!=(const JsonValueIterator&) const;

private:
  std::unique_ptr<Json::ValueIterator> iter_;
};

class JsonValue {
public:
  JsonValue();
  ~JsonValue();
  explicit JsonValue(const Json::Value& value);
  JsonValue(const JsonValue& value);
  explicit JsonValue(std::string path, const Json::Value& value);
  JsonValue& operator=(const JsonValue& value);
  JsonValueType type() const;

  [[deprecated("use as<std::string>() instead")]]
  std::string asString() const;
  [[deprecated("use as<StringView>() instead")]]
  StringView asStringView() const;
  [[deprecated("use as<bool>() instead")]]
  bool asBool() const;
  [[deprecated("use as<int32_t>() instead")]]
  int asInt() const;
  [[deprecated("use as<Decimal>() instead")]]
  Decimal asDecimal() const;
  [[deprecated("use as<double>() instead")]]
  double asDouble() const;
  [[deprecated("use as<std::string>(std::string) instead")]]
  std::string asString(const std::string& default_value) const;
  [[deprecated("use as<bool>(bool) instead")]]
  bool asBool(bool default_value) const;
  [[deprecated("use as<int32_t>(int32_t) instead")]]
  int asInt(int default_value) const;
  [[deprecated("use as<Decimal>(Decimal) instead")]]
  Decimal asDecimal(Decimal default_value) const;
  [[deprecated("use as<double>(double) instead")]]
  double asDouble(double default_value) const;
  [[deprecated("use as<std::vector<Decimal>>() instead")]]
  std::vector<Decimal> asDecimalVector() const;
  [[deprecated("use as<std::vector<int32_t>>() instead")]]
  std::vector<int> asIntVector() const;
  [[deprecated("use as<std::vector<std::string>>() instead")]]
  std::vector<std::string> asStringVector() const;
  [[deprecated("use as<std::vector<std::string>>(std::vector<std::string>) instead")]]
  std::vector<std::string> asStringVector(const std::vector<std::string>& default_value) const;

  template<typename T>
  T as() const {
    check_defined();
    WARNING_IF(empty()) << "Using empty json value at path: '" << path_ << "'";
    return unchecked_as(static_cast<const T*>(nullptr));
  }

  template<typename T>
  T as(const T& default_value) const {
    if (!defined()) {
      INFO() << "Using default json value: " << default_value << " at path: '" << path_ << "'";
      return default_value;
    }
    WARNING_IF(empty()) << "Using empty json value at path: '" << path_ << "'";
    return unchecked_as(static_cast<const T*>(nullptr));
  }

  JsonValue operator[] (size_t index) const;
  JsonValue operator[] (const std::string& key) const;
  JsonValue get(size_t index, const std::string& defaultValue) const;
  JsonValue get(const std::string& key, const std::string& defaultValue) const;
  void set(const std::string& key, const std::string& value);
  bool empty() const;
  JsonValueIterator begin() const;
  JsonValueIterator end() const;
  const std::string& path() const { return path_; }
  void merge(JsonValue other);
  std::vector<std::string> keys() const;
  std::string toStyledString() const;
  std::string toCompactString() const;
  bool is_member(const std::string& key) const;
  size_t size() const;
  Json::Value* get_raw() { return value_.get(); }

private:
  bool defined() const;
  void check_defined() const;
  std::string unchecked_as(const std::string*) const;
  StringView unchecked_as(const StringView*) const;
  bool unchecked_as(const bool*) const;
  int32_t unchecked_as(const int32_t*) const;
  int64_t unchecked_as(const int64_t*) const;
  Decimal unchecked_as(const Decimal*) const;
  double unchecked_as(const double*) const;

  template<typename T, typename R>
  auto unchecked_as(const std::chrono::duration<T, R>*) const {
    return std::chrono::duration<T, R>(unchecked_as(static_cast<const int32_t*>(nullptr)));
  }

  template<typename T>
  std::vector<T> unchecked_as(const std::vector<T>*) const {
    std::vector<T> result;
    result.reserve(size());
    std::transform(
      begin(),
      end(),
      std::back_inserter(result),
      [](JsonValue value) { return value.as<T>(); });
    return result;
  }

  std::string path_;
  std::unique_ptr<Json::Value> value_;
};

}  // namespace hftbattle
