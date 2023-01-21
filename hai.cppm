module;
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

export module hai;
import traits;
import :holder;
export import :cstr;
export import :sptr;
export import :uptr;

namespace hai {
template <> struct deleter<FILE> {
  void operator()(FILE *f) { fclose(f); }
};
export class c_file : holder<FILE> {
public:
  explicit c_file(const char *name, const char *mode) noexcept
      : holder{fopen(name, mode)} {}
  using holder<FILE>::operator*;
};

template <> struct deleter<void> {
  void operator()(void *f) { free(f); }
};
export class c_memory : holder<void> {
public:
  explicit c_memory(unsigned count, unsigned size) noexcept
      : holder{calloc(count, size)} {}
  using holder<void>::operator*;
};
} // namespace hai
