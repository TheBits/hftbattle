#pragma once

#include "base/string_stream.h"

namespace hftbattle {

class Security;
using SecurityId = const Security*;

StringStream& operator<<(StringStream& s, SecurityId id);

}  // namespace hftbattle
