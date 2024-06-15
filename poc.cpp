#pragma leco tool

import hai;

int main() {
  auto p = hai::uptr<int>::make(0);
  hai::fn<int> fn{[&] { return *p; }};
  return fn();
}
