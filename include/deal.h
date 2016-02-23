#pragma once

#include <vector>
#include <array>
#include <memory>
#include "base/decimal.h"
#include "base/constants.h"
#include "base/perf_time.h"

namespace hftbattle {
class Record;
class Order;
class CompressionInfoME;
class DeserializeHelper;

/**
 * Сделка - это акт купли-продажи определенного инструмента.
 * Класс Deal представляет собой реализацию биржевой сделки.
 **/
class Deal {
public:
  // Направление сделки (покупка или продажа).
  Dir dir;
  // Цена по которой произошла сделка.
  Price price;
  // Количество лотов в сделке.
  Amount amount;
  // Количество лотов, которые предположительно должны были свестись
  // в случае, когда налетающая заявка была нашей.
  Amount implied_amount;
  // Биржевое время совершения сделки.
  Microseconds server_time;
  // Время постановки "пассивного" ордера, если оно известно.
  Microseconds passive_order_server_time;
  // Локальное время в тиках.
  Ticks origin_time;
  // Биржевой (внешний) ID сделки.
  int64_t outer_id;

  // Направление налетающей заявки.
  inline Dir get_agressor_side() const {
    return dir;
  }

  // Цена, по которой произошла сделка.
  inline Price get_price() const {
    return price;
  }

  // Объем сделки.
  inline Amount get_amount() const {
    return amount;
  }

  // Биржевое время сделки.
  inline Microseconds get_server_time() const {
    return server_time;
  }

  // Локальное время сделки.
  inline Microseconds get_local_time() const {
    return Microseconds(origin_time.count() / Ticks::get_ticks_in_microsecond());
  }

  // Комментарии к заявкам, участвующим в сделке.
  std::array<std::string, 2> const& get_comments() const {
    return comments_;
  }

  // Вектор заявок (на продажу и на покупку), участвующих в сделке.
  std::array<std::shared_ptr<Order>, 2> const& get_orders() const {
    return orders_;
  }

  // Наша ли это сделка.
  bool is_our() const;

  /* Далее служебные методы. */

  Deal(CompressionInfoME* comp_info, DeserializeHelper* deserialize_helper);

  Deal(Price price, int amount, int implied_amount, int64_t moment, Dir agressor_side,
       std::shared_ptr<Order> order_ask, std::shared_ptr<Order> order_bid,
       const std::string& comment_ask, const std::string& comment_bid, int64_t tsc);

  Deal(std::shared_ptr<Record> record);

  ~Deal() { }

  int64_t get_moment() const;

  int64_t get_tsc() const;

  static std::vector<std::shared_ptr<Deal>> deserialize_list_with_compression(
      CompressionInfoME* comp_info, DeserializeHelper* deserialize_helper);

private:
  std::array<std::string, 2> comments_;
  std::array<std::shared_ptr<Order>, 2> orders_;
};

}  // namespace hftbattle
