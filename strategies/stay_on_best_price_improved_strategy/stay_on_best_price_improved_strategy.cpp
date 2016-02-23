#include "./participant_strategy.h"

using namespace hftbattle;

/**
 * Данная стратегия ставит заявку объемом 1 на лучшую цену,
 * если текущий объем на лучшей цене не меньше, чем min_volume_to_stay_on_best_,
 * и удаляет свою завявку в противном случае.
 **/

class UserStrategy : public ParticipantStrategy {
public:
  UserStrategy(JsonValue config) {
    min_volume_to_stay_on_best_ = config["min_volume_to_stay_on_best"].as<int>(10);
  }

  // Вызывается при получении нового стакана торгового инструмента:
  // @order_book – новый стакан.
  void trading_book_update(const OrderBook& order_book) override {
    auto our_orders = trading_book_info.orders();
    for (Dir dir: {BID, ASK}) {
      const Price best_price = trading_book_info.best_price(dir);
      const Amount best_volume = trading_book_info.best_volume(dir);
      const bool can_stay_on_best = best_volume >= min_volume_to_stay_on_best_;
      if (our_orders.active_orders_count(dir) == 0) {
        add_limit_order_if(dir, best_price, 1, can_stay_on_best);
      } else {  // есть хотя бы одна наша активная заявка
        auto first_order = our_orders.orders_by_dir[dir][0];
        const bool on_best_price = first_order->price == best_price;
        if (!on_best_price || !can_stay_on_best) {
          delete_order(first_order);
          add_limit_order_if(dir, best_price, 1, can_stay_on_best);
        }
      }
    }
  }

  void add_limit_order_if(Dir dir, Price price, Amount amount, bool condition) {
    if (condition) {
      add_limit_order(dir, price, amount);
    }
  }

private:
    Amount min_volume_to_stay_on_best_;
};


REGISTER_CONTEST_STRATEGY(UserStrategy, stay_on_best_price_improved_strategy)
