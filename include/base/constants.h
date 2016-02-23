#pragma once
#include "base/decimal.h"
#include "base/common_enums.h"

namespace hftbattle {
using Amount = int32_t;
using Price = Decimal;
using Id = uint64_t;

static constexpr double kMaxPrice = 100000000.0;
}  // namespace hftbattle
