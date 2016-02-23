#pragma once

#include <string>
#include "./security_id.h"
#include "base/constants.h"
#include "base/string_stream.h"
#include "base/common_enums.h"

namespace hftbattle {
class CompressionInfoME;
class DeserializeHelper;
class DeserializedOrder;
class AddOrderTransaction;

/**
 * Заявка - это пара <цена, количество лотов>.
 * Класс Order представляет собой реализацию биржевой заявки.
 **/
class Order {
public:
  // Наш (внутренний) ID заявки.
  const Id id;
  // Направление заявки.
  const Dir dir;
  // Цена заявки.
  const Price price;
  // Объем заявки (количество лотов).
  const Amount amount;
  // Тип заявки - Limit или IOC.
  const OrderTimeInForce time_in_force;
  // ID биржи.
  const MarketId market_id;
  // Инструмент, к которому относится заявка.
  const Security* security;
  // Максимальная длина комментария к заявке.
  static const size_t kMaxCommentLength = 20;

  // Текущий объем заявки. Может быть меньше начального, если были сделки с ее участием.
  inline Amount amount_rest() const {
    return amount_rest_;
  }

  // Биржевое время последнего изменения (в сотнях наносекунд).
  Microseconds get_server_time() const {
    return Microseconds(server_time);
  }

  // Количество лотов, которое предположительно будет сведено.
  inline Amount implied_amount() const {
    return implied_amount_;
  }

  // Статус заявки: в процессе добавления, активная, ждущая удаления, удаленная.
  inline OrderStatus status() const {
    return status_;
  }

  // Биржевой (внешний) ID заявки.
  inline Id outer_id() const {
    return outer_id_;
  }

  /* Далее служебные методы. */

  Order();

  Order(const AddOrderTransaction *transaction, OrderStatus status);

  Order(Id symbol_id, Id order_id,
        Dir dir, Price price, Amount amount, Amount implied_amount, int64_t moment_ticks,
        const std::string &comment = "",
        OrderTimeInForce orderType = OrderTimeInForce::Normal,
        Id ext_id = -1,
        OrderStatus status = OrderStatus::Active);

  Order(CompressionInfoME *comp_info, DeserializeHelper *deserialize_helper);

  Order(const DeserializedOrder& order);

  ~Order() { }

  void order_id(Id val);

//void comment(const std::string &comment) {
//  comment_ = comment;
//}

  const std::string &comment() const {
    return comment_;
  }

  bool is_our() const;

  int64_t delete_server_time() const {
    return delete_server_time_;
  }

  // Биржевое время постановки заявки в стакан в тиках.
  int64_t server_time;
  // Локальное время постановки заявки в стакан в тиках.
  const Ticks origin_time;

private:
  friend class OrderBookL3;
  friend class OrderBookVirtual;
  friend class ActiveOrders;
  friend class TradingSimulation;
  friend class StrategyLayer;
  friend class AddOrderTransaction;

  Id outer_id_;
  std::string comment_;
  OrderStatus status_;
  Amount amount_rest_;
  Amount implied_amount_;
  int64_t delete_server_time_;

  void amount_rest(Amount val) {
    amount_rest_ = val;
  }

  void implied_amount(Amount val) {
    implied_amount_ = std::max(val, 0);
  }

  void delete_server_time(int64_t val) {
    delete_server_time_ = val;
  }

  void status(OrderStatus val) {
    status_ = val;
  }
};

inline StringStream &operator<<(StringStream &s, const Order *id) {
  return id ? s << id->id : s << "nullptr";
}

inline StringStream &operator<<(StringStream &s, Order *id) {
  return id ? s << id->id : s << "nullptr";
}

inline StringStream &operator<<(StringStream &s, const Order &order) {
  s << "user_id: " << order.id <<
  " dir: " << order.dir <<
  " price: " << order.price <<
  " amount: " << order.amount <<
  " comment: " << order.comment();
  return s;
}

}  // namespace hftbattle
