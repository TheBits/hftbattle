#pragma once

#include "./order.h"
#include "base/constants.h"

namespace hftbattle {

typedef std::vector<Order*> SecurityOrders;

struct StatisticsSnapshot {
  Amount executed_amount;
  Amount implied_amount;

  StatisticsSnapshot(Amount executed_amount, Amount implied_amount)
      : executed_amount(executed_amount), implied_amount(implied_amount) {
  }

  StatisticsSnapshot()
      : executed_amount(0), implied_amount(0) {
  }

  Amount total_amount() const {
    return executed_amount + implied_amount;
  }
};

}  // namespace hftbattle
