#pragma once

#include "../traits/build.hpp"
#include "ecow.hpp"

inline auto hai() {
  using namespace ecow;

  auto m = unit::create<mod>("hai");
  m->add_wsdep("traits", traits());
  m->add_part("holder");
  m->add_part("sptr");
  m->add_part("uptr");
  m->add_part("cstr");
  return m;
}
