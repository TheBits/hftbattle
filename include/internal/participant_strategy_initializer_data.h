#pragma once

#include <base/json.h>

namespace hftbattle {

using Price = Decimal;

class ParticipantStrategyInitializerData {
public:
  static JsonValue get_config() { return *config_; }
  static void set(JsonValue config) { config_ = new JsonValue(config); }
private:
  static JsonValue* config_;
};

}  // namespace hftbattle
