#pragma once
#include <cstdint>
#include <chrono>
#include <limits>
#include <functional>

using namespace std::chrono_literals;
using std::chrono::duration_cast;

namespace hftbattle {

using Nanoseconds = std::chrono::nanoseconds;
using Microseconds = std::chrono::microseconds;
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;
using Minutes = std::chrono::minutes;
using Hours = std::chrono::hours;

class Ticks {
public:
  using base_type = int64_t;

  Ticks() = default;
  explicit constexpr Ticks(base_type count): count_(count) {
  }

  explicit constexpr operator bool() const { return count_; }
  constexpr base_type count() const { return count_; }

  static int64_t get_ticks_in_microsecond();
  static void init();

  Nanoseconds as_nanoseconds() const;

private:
  base_type count_;
};

inline bool operator==(Ticks lhs, Ticks rhs) { return lhs.count() == rhs.count(); }
inline bool operator!=(Ticks lhs, Ticks rhs) { return lhs.count() != rhs.count(); }
inline bool operator>(Ticks lhs, Ticks rhs) { return lhs.count() > rhs.count(); }
inline bool operator<(Ticks lhs, Ticks rhs) { return lhs.count() < rhs.count(); }
inline bool operator>=(Ticks lhs, Ticks rhs) { return lhs.count() >= rhs.count(); }
inline bool operator<=(Ticks lhs, Ticks rhs) { return lhs.count() <= rhs.count(); }
inline Ticks operator-(Ticks lhs, Ticks rhs) { return Ticks(lhs.count() - rhs.count()); }

Ticks rdtsc();

Nanoseconds nanoseconds_distance(Ticks t1, Ticks t2);
Nanoseconds nanoseconds_distance(Ticks start);
Microseconds microseconds_distance(Ticks t1, Ticks t2);
Microseconds microseconds_distance(Ticks start);

}  // namespace hftbattle

namespace std {
template<>
class numeric_limits<hftbattle::Ticks> {
public:
  static constexpr hftbattle::Ticks max() {
    return hftbattle::Ticks(numeric_limits<hftbattle::Ticks::base_type>::max());
  }
  static constexpr hftbattle::Ticks min() {
    return hftbattle::Ticks(numeric_limits<hftbattle::Ticks::base_type>::min());
  }
};

template <>
struct hash<hftbattle::Ticks> {
  size_t operator()(hftbattle::Ticks d) const {
    return hash<int64_t>()(d.count());
  }
};

}  // std
