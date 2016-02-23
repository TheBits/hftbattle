#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include "./quote.h"
#include "base/log.h"
#include "internal/quotes_holder.h"

namespace hftbattle {

class Transaction;
class AddOrderTransaction;
class DelOrderTransaction;
class RecordL1;
class Record;
class Deal;
class QuotesHolder;

/**
 * Биржевой стакан - это агрегатор всех заявок по конкретному инструменту.
 * Класс OrderBook представляет собой реализацию биржевого стакана.
 * ! Для индексов в стакане используется 0-нумерация, начиная от лучшей цены.
 **/
class OrderBook {
using Snapshot = OrderBook;

public:
  // Котировка с индексом @index в стакане по направлению @dir.
  const Quote& get_quote_by_index(Dir dir, int index) const;

  // Цена котировки с индексом @index по направлению @dir.
  inline Price get_price_by_index(Dir dir, int index) const {
    return get_quote_by_index(dir, index).get_price();
  }

  // Суммарный объем лотов котировки с индексом @index по направлению @dir.
  inline Amount get_volume_by_index(Dir dir, int index) const {
    return get_quote_by_index(dir, index).get_volume();
  }

  // Биржевое время последнего изменения котировки с индексом @index по направлению @dir.
  inline int64_t get_quote_server_time_by_index(Dir dir, int index) const {
    return get_quote_by_index(dir, index).get_last_moment_ticks();
  }

  // Котировка по цене @price по направлению @dir.
  const Quote& get_quote_by_price(Dir dir, Price price) const;

  // Индекс котировки с ценой @price по направлению @dir.
  size_t get_index_by_price(Dir dir, Price price) const;

  // Суммарный объем лотов на цене @price по направлению @dir.
  inline Amount get_volume_by_price(Dir dir, Price price) const {
    return get_quote_by_price(dir, price).get_volume();
  }

  // Биржевое время последнего изменения котировки по цене @price по направлению @dir.
  inline int64_t get_quote_server_time_by_price(Dir dir, Price price) const {
    return get_quote_by_price(dir, price).get_last_moment_ticks();
  }

  // Лучшая цена в стакане по направлению @dir.
  inline Price best_price(Dir dir) const {
    return get_price_by_index(dir, 0);
  }

  // Суммарный объем лотов на лучшей цене по направлению @dir.
  inline Amount best_volume(Dir dir) const {
    return get_volume_by_index(dir, 0);
  }

  // Есть ли в стакане цена @price по направлению @dir.
  bool contains_price(Dir dir, Price price) const;

  // Все котировки по направлению @dir.
  QuotesHolder all_quotes(Dir dir) const;

  // Количество котировок по направлению @dir.
  virtual size_t quotes_count(Dir dir) const;

  // Максимальная по направлениям глубина отображаемого стакана.
  size_t depth() const;

  // Биржевое время последнего изменения стакана, в микросекундах
  inline Microseconds get_server_time() const {
    return Microseconds(last_moment_ticks_);
  }

  // Время последнего изменения стакана на машине, которая получала данные от биржи, в микросекундах
  inline Microseconds get_local_time() const {
    return Microseconds(last_tsc_ / Ticks::get_ticks_in_microsecond());
  }

  /* Далее служебные методы. */

  void build_snapshot(Snapshot* snapshot) const;

  void remove_quote_by_index(Dir dir, int index);

  virtual void add_record(const Record* record, int64_t last_tsc, int64_t lastReceivedTSC);

  void clear_quotes();

  virtual void modify_quote_volume(Quote* quote, int amount_diff);

  OrderBook(const OrderBook *orderBook);

  Quote* quote_by_price(Dir dir, Price price);

  Quote* quote_by_index(Dir dir, int index);

  inline int64_t get_last_tsc() const {
    return last_tsc_;
  };

  inline int64_t get_last_moment_ticks() const {
    return last_moment_ticks_;
  };

  void set_last_moment_ticks(int64_t last_moment_ticks);

  void set_last_tsc(int64_t lastTSC);

  virtual int64_t get_last_received_tsc() const;

  bool with_trades() const {
    return false;
  }

  QuotesHolder get_bids() const {
    return all_quotes(Dir::BID);
  }

  QuotesHolder get_asks() const {
    return all_quotes(Dir::ASK);
  }

  OrderBook(Id security_id, size_t snapshot_depth);
  virtual ~OrderBook() = default;

protected:
  int64_t last_received_tsc_ = -1;
  std::array<QuotesType, 2> quotes_;
  virtual std::unique_ptr<Quote> create_quote(Dir dir, Decimal price);

private:
  int64_t last_moment_ticks_ = 0;
  int64_t last_tsc_ = 0;
  Id security_id_;
  size_t snapshot_depth_ = 0;
};

}  // namespace hftbattle
