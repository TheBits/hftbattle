#pragma once

#include <cstdint>

namespace hftbattle {

enum class SecurityType : uint8_t {
  Future,
  Count,
  All = Count
};

enum Dir : uint8_t {
  BUY = 0,
  BID = 0,
  SELL = 1,
  ASK = 1,
  UNKNOWN = 3
};

enum class OrderStatus : int8_t {
  Adding, Active, Deleting, Deleted
};


enum class OrderTimeInForce : int8_t {
  Normal = 0,
  ImmediateOrCancel,
  FillOrKill
};

enum class FilterResult : int8_t {
  Older = 0,
  Same,
  InsignificantlyNewer,
  SignificantlyNewer,
  Count
};

enum class MarketId : uint8_t {
  CME,
  Count,
  All = Count
};

inline Dir opposite_dir(Dir dir) {
  return static_cast<Dir>(1 - dir);
}

// BID/BUY = 1, SELL/ASK = -1
inline int32_t dir_sign(Dir dir) {
  return static_cast<int8_t>(dir) * -2 + 1;
}

enum class ChartYAxisType : int8_t {
  Left,
  Right
};

}  // namespace hftbattle
