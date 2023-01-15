#pragma once

#include "ecow.hpp"

inline auto hai() {
  using namespace ecow;

  auto m = unit::create<mod>("hai");
  return m;
}
