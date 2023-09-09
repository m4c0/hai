#pragma once

#include "../ecow/ecow.hpp"

inline auto hai() {
  using namespace ecow;
  return unit::create<box>("hai");
}
