#include "./participant_strategy.h"

using namespace hftbattle;

/**
 * Данная стратегия - модификация стратегии deals_count_diff_strategy.
 * Здесь мы хотим ограничить суммарный объем наших сделок,
 * используя информацию, приходящую в функции execution_report_update.
 **/
class UserStrategy : public ParticipantStrategy {
public:
  UserStrategy(JsonValue config)
  : last_reset_time_(0)
  , our_deals_total_amount_(0)
  , trading_finished_(false)
  {
    deals_count_by_dir_.fill(0);
    min_deals_count_diff_ = config["min_deals_count_diff"].as<int>(100);
    deals_reset_period_ms_ = config["deals_reset_period_ms"].as<Milliseconds>(10ms);
    our_deals_max_total_amount_ = config["our_deals_max_total_amount"].as<Amount>(1000);

    std::cout << "min_deals_count_diff_: " << min_deals_count_diff_ << std::endl;
    std::cout << "deals_reset_period_ms_: " << deals_reset_period_ms_.count() << std::endl;
    std::cout << "our_deals_max_total_amount_: " << our_deals_max_total_amount_ << std::endl;
  }

  // Вызывается при получении нового стакана торгового инструмента:
  // @order_book – новый стакан.
  void trading_book_update(const OrderBook& order_book) override {
    /* написать свою реализацию здесь */
  }

  // Вызывается при получении новых сделок торгового инструмента:
  // @deals - вектор новых сделок.
  void trading_deals_update(const std::vector<Deal>& deals) override {
    if (trading_finished_) {
      return;
    }

    for (const auto& deal : deals) {
      deals_count_by_dir_[deal.dir] += 1;
    }
    const int deals_count_diff = std::abs(deals_count_by_dir_[ASK] -
                                          deals_count_by_dir_[BID]);
    bool trade = deals_count_diff >= min_deals_count_diff_;
    if (trade) {
      const Dir dir_to_beat = deals_count_by_dir_[ASK] >= deals_count_by_dir_[BID]
                              ? ASK
                              : BID;
      const Price price_to_beat = trading_book_info.best_price(opposite_dir(dir_to_beat));
      const Amount amount_to_beat = 1;
      add_ioc_order(dir_to_beat, price_to_beat, amount_to_beat);
    }

    const Microseconds current_time = get_server_time();
    const Microseconds time_diff_us = current_time - last_reset_time_;
    // Далее сравнивается величина time_diff_us в микросекундах
    // с величиной deals_reset_period_ms_ в миллисекундах -
    // это нормально, система сама понимает как сравнить два времени,
    // выраженные в разных единицах измерения.
    if (trade || time_diff_us >= deals_reset_period_ms_) {
      last_reset_time_ = current_time;
      deals_count_by_dir_[BID] = 0;
      deals_count_by_dir_[ASK] = 0;
    }
  }

  // Вызывается при получении отчета о сделке с участием вашего ордера:
  // @execution_report – структура-отчет о совершенной сделке.
  void execution_report_update(const ExecutionReport& execution_report) override {
    our_deals_total_amount_ += execution_report.deal_amount();
    if (our_deals_total_amount_ > our_deals_max_total_amount_) {
      trading_finished_ = true;
    }
  }

private:
  Microseconds last_reset_time_;
  Amount our_deals_total_amount_;
  bool trading_finished_;
  std::array<int, 2> deals_count_by_dir_;

  int min_deals_count_diff_;
  Milliseconds deals_reset_period_ms_;
  Amount our_deals_max_total_amount_;
};

REGISTER_CONTEST_STRATEGY(UserStrategy, deals_count_diff_with_limit_strategy)
