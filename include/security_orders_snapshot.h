#pragma once

#include "internal/order_snapshot.h"
#include <numeric>

namespace hftbattle {

/**
 * Класс SecurityOrdersSnapshot хранит текущие заявки стратегии.
 * Структура обновляется перед приходом каждого апдейта в стратегию.
 * В процессе обработки одного апдейта структура гарантированно не меняется.
 **/
class SecurityOrdersSnapshot {
public:
  // Списки наших текущих заявок по направлению.
  std::array<std::vector<OrderSnapshot>, 2> orders_by_dir;
  // Суммарный объем заявок по направлению, отправленных на удаление, но еще не удаленных.
  std::array<Amount, 2> deleting_amount;

  SecurityOrdersSnapshot() {
    for (Dir dir : {BID, ASK}) {
      orders_by_dir[dir].reserve(100);
      deleting_amount[dir] = 0;
    }
  }

  // Суммарный объём заявок с заданной ценой @price по направлению @dir.
  Amount get_volume_by_price(Dir dir, Decimal price) const {
    return std::accumulate(orders_by_dir[dir].cbegin(), orders_by_dir[dir].cend(), 0,
        [price](const Amount volume, const OrderSnapshot& order) {
            return volume + (order->status() == OrderStatus::Active && order->price == price ? order->amount_rest() : 0);
        });
  }

  // Количество наших активных заявок по направлению @dir.
  size_t active_orders_count(Dir dir) const {
    return static_cast<size_t>(std::count_if(orders_by_dir[dir].cbegin(), orders_by_dir[dir].cend(),
        [](const OrderSnapshot& order) {
            return order->status() == OrderStatus::Active;
        }));
  }

  // Возвращает суммарный объем активных ордеров по направлению @dir.
  Amount active_orders_volume(Dir dir) const {
    return std::accumulate(orders_by_dir[dir].cbegin(), orders_by_dir[dir].cend(), 0,
        [](const Amount volume, const OrderSnapshot& order) {
            return volume + (order->status() == OrderStatus::Active ? order->amount_rest() : 0);
        });
  }

  // Суммарное количество наших заявок по обоим направлениям.
  size_t size() const {
    return orders_by_dir[Dir::BID].size() + orders_by_dir[Dir::ASK].size();
  }

private:
  friend class OrdersStorage;

  void clear();

};

}  // namespace hftbattle
