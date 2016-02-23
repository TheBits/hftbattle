#pragma once

#include "./order.h"

namespace hftbattle {

class ExecutionReport {
public:
  ExecutionReport(
      Microseconds server_time, const Order* order, Price deal_price, Amount deal_amount):
    server_time_(server_time), order_(order), deal_price_(deal_price), deal_amount_(deal_amount) {}

  Microseconds server_time() const { return server_time_; }
  const Order* order() const { return order_; }
  Price deal_price() const { return deal_price_; }
  Amount deal_amount() const{ return deal_amount_; }
  Dir dir() const { return order_->dir; }

private:
  const Microseconds server_time_;
  const Order* order_;
  const Price deal_price_;
  const Amount deal_amount_;
};

}  // namespace hftbattle
