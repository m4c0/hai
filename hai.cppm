module;
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

export module hai;

namespace hai {
export class c_file {
  FILE *m_file;

  void close() {
    if (m_file != nullptr)
      fclose(m_file);

    m_file = nullptr;
  }

public:
  explicit c_file(const char *name, const char *mode) noexcept
      : m_file{fopen(name, mode)} {}
  ~c_file() noexcept { close(); }

  c_file(const c_file &) = delete;
  c_file &operator=(const c_file &) = delete;

  c_file(c_file &&o) : m_file(o.m_file) {}
  c_file &operator=(c_file &&o) {
    close();
    m_file = o.m_file;
    return *this;
  }

  [[nodiscard]] constexpr auto operator*() noexcept { return m_file; }
};

export class c_memory {
  void *m_ptr;

  void close() {
    if (m_ptr != nullptr)
      free(m_ptr);

    m_ptr = nullptr;
  }

public:
  explicit c_memory(unsigned count, unsigned size) noexcept
      : m_ptr{calloc(count, size)} {}
  ~c_memory() noexcept { close(); }

  c_memory(const c_memory &) = delete;
  c_memory &operator=(const c_memory &) = delete;

  c_memory(c_memory &&o) : m_ptr(o.m_ptr) {}
  c_memory &operator=(c_memory &&o) {
    close();
    m_ptr = o.m_ptr;
    return *this;
  }

  [[nodiscard]] constexpr auto operator*() noexcept { return m_ptr; }
};
} // namespace hai
