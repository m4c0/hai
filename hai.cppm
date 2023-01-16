module;
#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

export module hai;

namespace hai {
template <typename Tp> struct deleter {
  constexpr void operator()(Tp f) { delete[] f; }
};
export template <typename Tp> class holder {
  Tp m_ptr;

  constexpr void reset() {
    if (m_ptr != nullptr)
      deleter<Tp>{}(m_ptr);

    m_ptr = nullptr;
  }

protected:
  explicit constexpr holder(Tp p) noexcept : m_ptr{p} {}

public:
  constexpr ~holder() noexcept { reset(); }

  holder(const holder &) = delete;
  holder &operator=(const holder &) = delete;

  constexpr holder(holder &&o) : m_ptr(o.m_ptr) {}
  constexpr holder &operator=(holder &&o) {
    reset();
    m_ptr = o.m_ptr;
    return *this;
  }

  [[nodiscard]] constexpr auto &operator*() noexcept { return m_ptr; }
  [[nodiscard]] constexpr const auto &operator*() const noexcept {
    return m_ptr;
  }
};

export template <typename Tp> class memory : holder<Tp *> {
  unsigned m_count;

public:
  constexpr explicit memory(unsigned count) noexcept
      : holder<Tp *>{new Tp[count]}, m_count{count} {}

  using holder<Tp *>::operator*;

  [[nodiscard]] constexpr auto &operator[](unsigned index) {
    assert(index >= 0 && index < m_count);
    return (**this)[index];
  }
};

template <> struct deleter<FILE *> {
  void operator()(FILE *f) { fclose(f); }
};
export class c_file : holder<FILE *> {
public:
  explicit c_file(const char *name, const char *mode) noexcept
      : holder{fopen(name, mode)} {}
  using holder<FILE *>::operator*;
};

template <> struct deleter<void *> {
  void operator()(void *f) { free(f); }
};
export class c_memory : holder<void *> {
public:
  explicit c_memory(unsigned count, unsigned size) noexcept
      : holder{calloc(count, size)} {}
  using holder<void *>::operator*;
};

export template <typename Tp> class uptr : holder<Tp *> {
public:
  template <typename... Args>
  constexpr uptr(Args &&...args) : holder<Tp *>{new Tp{args...}} {}

  [[nodiscard]] constexpr Tp &operator*() noexcept {
    return *(holder<Tp *>::operator*());
  }
  [[nodiscard]] constexpr const Tp &operator*() const noexcept {
    return *(holder<Tp *>::operator*());
  }
  [[nodiscard]] constexpr Tp *operator->() noexcept {
    return holder<Tp *>::operator*();
  }
  [[nodiscard]] constexpr const Tp *operator->() const noexcept {
    return holder<Tp *>::operator*();
  }
};
} // namespace hai
