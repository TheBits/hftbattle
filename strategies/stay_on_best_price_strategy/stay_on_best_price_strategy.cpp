#include "./participant_strategy.h"

using namespace hftbattle;

/**
 * Данная стратегия стоит на каждом направлении (BID и ASK)
 * одним лотом на лучшей цене.
 **/
class UserStrategy : public ParticipantStrategy {
public:
  UserStrategy(JsonValue config) {}

  // Вызывается при получении нового стакана торгового инструмента:
  // @order_book – новый стакан.
  void trading_book_update(const OrderBook& order_book) override {
    auto our_orders = trading_book_info.orders();
    for (Dir dir: {BID, ASK}) {
      const Price best_price = trading_book_info.best_price(dir);
      const Amount amount = 1;
      if (our_orders.active_orders_count(dir) == 0) {
        add_limit_order(dir, best_price, amount);
      } else {  // есть хотя бы одна наша активная заявка
        auto first_order = our_orders.orders_by_dir[dir][0];
        const bool on_best_price = first_order->price == best_price;
        if (!on_best_price) {  // наша заявка стоит, но не на текущей лучшей цене
          delete_order(first_order);
          add_limit_order(dir, best_price, amount);
        }
      }
    }
  }

};

REGISTER_CONTEST_STRATEGY(UserStrategy, stay_on_best_price_strategy)