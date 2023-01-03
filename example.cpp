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

int main() { return use_c_file(); }
