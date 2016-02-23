#include "./participant_strategy.h"

using namespace hftbattle;

/**
 * Данная стратегия торгует на основе произошедших на бирже сделок:
 * если абсолютная разность количества сделок на биде и аске
 * за определенное время (deals_reset_period_ms_) больше некоторой
 * фиксированной величины (min_deals_count_diff_), то стратегия выставляет заявку
 * типа Immediate-Or-Cancel (IOC) по преобладающему направлению и обнуляет счетчики.
 * Замечание 1: направление сделки - это направление агрессора,
 * то есть активной стороны, спровоцировавшей сделку.
 * Замечание 2: заявка типа IOC сразу после выполнения всех возможный сведений
 * автоматически удаляется из стакана, в отличие от обычной заявки,
 * которая остается в стакане после выполнения всех возможных сведений.
 **/
class UserStrategy : public ParticipantStrategy {
public:
  UserStrategy(JsonValue config)
  : last_reset_time_(0)
  {
    deals_count_by_dir_.fill(0);
    min_deals_count_diff_ = config["min_deals_count_diff"].as<int>(100);
    deals_reset_period_ms_ = config["deals_reset_period_ms"].as<Milliseconds>(10ms);

    std::cout << "min_deals_count_diff_: " << min_deals_count_diff_ << std::endl;
    std::cout << "deals_reset_period_ms_: " << deals_reset_period_ms_.count() << std::endl;
  }

  // Вызывается при получении новых сделок торгового инструмента:
  // @deals - вектор новых сделок.
  void trading_deals_update(const std::vector<Deal>& deals) override {
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

private:
  Microseconds last_reset_time_;
  std::array<int, 2> deals_count_by_dir_;

  int min_deals_count_diff_;
  Milliseconds deals_reset_period_ms_;
};

REGISTER_CONTEST_STRATEGY(UserStrategy, deals_count_diff_strategy)
