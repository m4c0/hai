#include <stdio.h>

import hai;

int use_c_file() {
  hai::c_file f{"example.cpp", "rb"};
  if (!*f)
    return 1;

  hai::c_memory buf{sizeof(int), 32};
  if (fread(*buf, sizeof(int), 32, *f) != 32)
    return 2;

  return 0;
}
int use_cpp_mem() {
  hai::memory<long> b{5};
  if (!*b)
    return 8;

  b[0] = b[1] = b[4] + 4;
  return 0;
}
auto use_uptr() {
  // Just check if it is syntatically logical
  auto p = [] {
    hai::uptr<long> p{42l};
    return p;
  }();
  p = hai::uptr<long>{22l};
  *p = 99;
  return p;
}

int main() { return use_c_file() + use_cpp_mem(); }
