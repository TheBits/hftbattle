#pragma once

#include "./order_book.h"
#include "./security_orders_snapshot.h"
#include "./security_id.h"
#include "base/constants.h"
#include "internal/statistics_snapshot.h"

namespace hftbattle {

/**
 * Класс ContestBookInfo служит агрегатором информации по текущему состоянию стакана.
 * Он не является полноценной заменой стакана, но содержит часть информации о нем.
 * Предпочтительно пользоваться именно этим классом, так как это значительно быстрее,
 * чем запрашивать у стакана.
 **/
class ContestBookInfo {
public:
  // Лучшая цена в стакане по направлению @dir.
  const Price best_price(Dir dir) const { return best_price_by_dir_[dir]; }

  // Суммарный объем лотов на лучшей цене по направлению @dir.
  const Amount best_volume(Dir dir) const { return best_volume_by_dir_[dir]; }

  // Наша текущая позиция.
  Amount executed_amount() const { return statistics_.executed_amount; }

  // Наша общая позиция: учитывается как реальная позиция на руках, так и предполагаемая.
  Amount total_amount() const { return statistics_.executed_amount + statistics_.implied_amount; }

  // Ссылка на структуру, содержащая наши текущие заявки.
  SecurityOrdersSnapshot& orders() { return orders_; }

  // Полусумма лучших цен.
  Price middle_price() const { return middle_price_; }

  // Минимальный шаг цены в стакане (минимальная возможная разница между ценами).
  Price min_step() const { return min_step_; }

  // Расстояние между лучшим аском и лучшим бидом в минимальных шагах цены.
  int32_t spread_in_min_steps() const { return spread_in_min_steps_; }

  // Количество апдейтов стакана с начала дня.
  int32_t book_updates_count() const { return book_updates_count_; }

  // Биржевое время последнего обновления.
  Microseconds server_time() const { return server_time_; }

  // Инструмент, которому соответствует данный класс.
  SecurityId security_id() const { return security_id_; }

  // Структура, содержащая статистику по нашей текущей позиции.
  StatisticsSnapshot statistics() { return statistics_; }

  /* Далее служебные методы. */

  ContestBookInfo(SecurityId security);

  void set_statistics(StatisticsSnapshot statistics) { statistics_ = statistics; }

  void update(const OrderBook* book, bool is_orderbookL1);

private:
  SecurityId security_id_;
  Price middle_price_;
  int32_t spread_in_min_steps_;
  int32_t book_updates_count_;
  Microseconds server_time_;
  Price min_step_;
  std::array<Price, 2> best_price_by_dir_;
  std::array<int64_t, 2> best_volume_by_dir_;
  SecurityOrdersSnapshot orders_;
  StatisticsSnapshot statistics_;

};

}  // namespace hftbattle
