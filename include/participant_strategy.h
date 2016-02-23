#pragma once

#include <iostream>
#include "./contest_book_info.h"
#include "./execution_report.h"
#include "./order_book.h"
#include "./deal.h"
#include "base/macroses.h"
#include "internal/participant_strategy_initializer_data.h"

namespace hftbattle {

class Strategy;
class DataFeedSnapshot;

/**
 * ParticipantStrategy - класс-интерфейс для написания пользовательских стратегий.
 * Этот класс служит прослойкой между симуляционным ядром и стратегией.
 *
 * Он обеспечивает обработку входящих сигналов от симуляции:
 * обновления торгового стакана, сделок, отчеты о наших сделках.
 * Он же осуществляет передачу исходящих сигналов в симуляцию: постановка и снятие заявок.
 *
 * Помимо реализации методов для описанных выше действий,
 * класс также предоставляет некоторые вспомогательные методы для удобства работы.
 *
 * Классы-стратегии участников должны наследовать от класса ParticipantStrategy.
 **/
class ParticipantStrategy {
private:
  friend class Strategy;
  friend class RoyalManagerContest;
  friend class RoyalManagerBase;

  std::unique_ptr<Strategy> strategy_;

  // Метод-обертка, в котором реализована логика обработки биржевых событий.
  // Вызывает соответствующие методы-апдейты участника.
  void trade(std::shared_ptr<DataFeedSnapshot>&& snapshot);

  // Информация о торговом и сигнальном инструментах.
  const SecurityId trading_security_;
  const SecurityId signal_security_;
  // Текущее биржевое время.
  struct tm current_update_server_time_tm_;
  // Максимально разрешенная позиция. По умолчанию равняется 50.
  Amount max_total_amount_;

public:
    /* Методы, которые участник может реализовать. */

  // Вызывается при получении нового стакана торгового инструмента:
  // @order_book – новый стакан.
  virtual void trading_book_update(const OrderBook& order_book) {
    /* написать свою реализацию здесь */
  }

  // Вызывается при получении новых сделок торгового инструмента:
  // @deals - вектор новых сделок.
  virtual void trading_deals_update(const std::vector<Deal>& deals) {
    /* написать свою реализацию здесь */
  }

  // Вызывается при получении отчета о сделке с участием вашего ордера:
  // @snapshot – структура-отчет о совершенной сделке.
  virtual void execution_report_update(const ExecutionReport& execution_report) {
    /* написать свою реализацию здесь */
  }

  // Вызывается при получении нового стакана сигнального инструмента:
  // @order_book – новый стакан.
  virtual void signal_book_update(const OrderBook& order_book) {
    /* написать свою реализацию здесь */
  }

  // Вызывается при получении новых сделок сигнального инструмента:
  // @deals - вектор новых сделок.
  virtual void signal_deals_update(const std::vector<Deal>& deals) {
    /* написать свою реализацию здесь */
  }

  /* Методы класса, которые участник может использовать. */

  // Выставляет нашу лимитную заявку:
  // @dir - направление (BID = 0 - покупка, ASK = 1 - продажа),
  // @price - цена, по которой заявка будет выставлена,
  // @amount - размер заявки.
  // @comment - комментарий к заявке.
  bool add_limit_order(Dir dir, Price price, Amount amount, const std::string& comment = {});

  // Выставляет нашу заявку типа Immediate-Or-Cancel (IOC):
  // @dir - направление (BID = 0 - покупка, ASK = 1 - продажа),
  // @price - цена, по которой заявка будет выставлена,
  // @amount - размер заявки.
  // @comment - комментарий к заявке.
  bool add_ioc_order(Dir dir, Price price, Amount amount, const std::string& comment = {});

  // Снимает нашу заявку с торгов:
  // @order - заявка, которую мы хотим снять.
  void delete_order(Order* order);

  // Снимает все наши заявки с торгов по направлению @dir.
  // @dir - направление (BID = 0 - покупка, ASK = 1 - продажа).
  void delete_all_orders_by_dir(Dir dir);

  // Возвращает количество лотов, стоящих в очереди перед нашей заявкой.
  // @order - заявка, для которой мы хотим узнать количество стоящих перед ней лотов.
  Amount get_amount_before_order(Order* order) const;

  // Добавляет точку на график.
  // @line_name - название графика, @value - значение, @y_axis_type - ось (левая или правая),
  // @chart_number - номер картинки на которой будет отрисован график
  void add_chart_point(const std::string& line_name,
                                 double value,
                                 ChartYAxisType y_axis_type,
                                 uint8_t chart_number);

  // Возвращает текущий результат (заработок).
  Price get_current_result() const;

  // Возвращает текущее сальдо, т.е. баланс без учета позы.
  Price get_saldo();

  // Возвращает true, если есть сигнальный инструмент. Иначе false.
  bool signal_security_exists() const;

  // Возвращает локальное время в микросекундах.
  // Локальное время здесь – это время на машине, получающей биржевые данные.
  Microseconds get_local_time() const;

  // Возвращает биржевое время с точностью до микросекунд.
  Microseconds get_server_time() const;

  // Возвращает биржевое время типа tm c точностью до секунды.
  tm get_server_time_tm() const;

  // Устанавливает максимальное разрешённое значение позиции (не более 50).
  void set_max_total_amount(const uint32_t max_total_amount);

  // Устанавливает минимально допустимое значение, при достижении которого позиция закрывается, и стратегия перестаёт торговать.
  void set_stop_loss_result(const Decimal stop_loss_result);

  /* Поля класса, которые участник может использовать. */

  // Стуктура-аггрегатор основной информации о торговом стакане.
  ContestBookInfo trading_book_info;
  // Стуктура-аггрегатор основной информации о сигнальном стакане.
  ContestBookInfo signal_book_info;
  // Умный указатель на текущий стакан торгового инструмента.
  // Они обновляются каждый раз с приходом очередного апдейта торгового стакана.
  // При этом объект внутри (стакан) разрушается.
  // Чтобы сохранить старый стакан, нужно явно в стратегии сохранить этот указатель.
  std::shared_ptr<const OrderBook> trading_book;
  // Аналогично trading_book для сигнального инструмента.
  std::shared_ptr<const OrderBook> signal_book;

  /* Конструкторы. */
  ParticipantStrategy();
  explicit ParticipantStrategy(JsonValue config);

  /* Деструктор. */
  virtual ~ParticipantStrategy();
};

#define REGISTER_CONTEST_STRATEGY(ClassName, file) \
extern "C" ParticipantStrategy* make_ ## file (JsonValue config) { \
  ParticipantStrategyInitializerData::set(config); \
  return new UserStrategy(config); \
} \

}  // namespace hftbattle
