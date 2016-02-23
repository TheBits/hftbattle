#pragma once

#include "base/constants.h"
#include "base/perf_time.h"

namespace hftbattle {

inline Price default_quote_price(Dir dir) {
  return (dir == Dir::BID) ? Price(0.0) : Price(kMaxPrice);
}
/**
 * Котировка (или квота) - это термин для обозначения одного ценового уровня в стакане.
 * Она характеризуется
 * - направлением (покупка или продажа),
 * - ценой,
 * - объемом, т.е. количеством лотов, которые доступны для покупки/продажи по данной цене.
 * В стратегии предлагается использовать один из методов класса OrderBook для получения
 * объекта типа Quote (например, get_quote_by_index).
 **/
class Quote {
public:
  // Направление котировки.
  inline Dir get_dir() const {
    return dir_;
  }

  // Цена котировки.
  inline Price get_price() const {
    return price_;
  }

  // Объем лотов котировки.
  inline Amount get_volume() const {
    return volume_;
  }

  // Биржевое время последнего изменения в микросекундах.
  Microseconds get_server_time() const {
    return Microseconds(last_moment_ticks_);
  }

  // Локальное время последнего изменения в микросекундах.
  Microseconds get_local_time() const {
    return Microseconds(last_tsc_ / Ticks::get_ticks_in_microsecond());
  }

  /* Далее служебные методы. */

  int64_t get_last_moment_ticks() const {
    return last_moment_ticks_;
  }

  int64_t get_last_tsc() const {
    return last_tsc_;
  }

  void set_last_moment_ticks(int64_t last_moment_ticks) {
    last_moment_ticks_ = last_moment_ticks;
  }

  void set_last_tsc(int64_t last_tsc) {
    last_tsc_ = last_tsc;
  }

  virtual void modify_volume(int amount_diff, int64_t last_moment_ticks, int64_t last_tsc);

  explicit Quote(Dir dir) :
    dir_(dir),
    price_(default_quote_price(dir)),
    last_moment_ticks_(0),
    last_tsc_(0),
    volume_(0) {
  }

  Quote(Dir dir, Price price, int64_t last_moment_ticks, int64_t last_tsc, Amount volume = 0) :
    dir_(dir),
    price_(price),
    last_moment_ticks_(last_moment_ticks),
    last_tsc_(last_tsc),
    volume_(volume) {
  }

  virtual ~Quote() = default;
  Quote& operator=(Quote&&)= default;
  Quote(Quote&&) = default;
  Quote(const Quote&) = default;
  Quote& operator=(const Quote&) = default;

protected:
  Dir dir_;
  Price price_;
  int64_t last_moment_ticks_;
  int64_t last_tsc_;
  Amount volume_;
};

}  // namespace hftbattle
