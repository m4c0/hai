#pragma once

#include "../traits/build.hpp"
#include "ecow.hpp"

inline auto hai() {
  using namespace ecow;

  auto m = unit::create<mod>("hai");
  m->add_wsdep("traits", traits());
  return m;
}
