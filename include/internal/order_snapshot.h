#pragma once

#include "./order.h"

namespace hftbattle {

class OrderSnapshot {
public:
  explicit OrderSnapshot(Order* order) :
      order_(order), amount_rest_(order_->amount_rest()) {
  }

  Amount amount_rest() const {
    return amount_rest_;
  }

  operator Order*() {
    return order_;
  }

  const Order* operator->() const {
    return order_;
  }

private:
  Order* order_;
  const Amount amount_rest_;
};

}  // namespace hftbattle
