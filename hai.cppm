module;
#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// This is only required when compiling on MacOSX, but it fails on Android (for
// reasons)
#ifdef __APPLE__
#include <new>
#endif

export module hai;
import traits;

namespace hai {
template <typename Tp> struct deleter {
  constexpr void operator()(Tp *f) const noexcept { delete f; }
};
template <> struct deleter<FILE> {
  void operator()(FILE *f) { fclose(f); }
};
template <> struct deleter<void> {
  void operator()(void *f) { free(f); }
};

export template <typename Tp> class holder {
  Tp *m_ptr;

  constexpr void reset() {
    if (m_ptr != nullptr)
      deleter<Tp>{}(m_ptr);

    m_ptr = nullptr;
  }

public:
  constexpr holder() noexcept = default;
  explicit constexpr holder(Tp *p) noexcept : m_ptr{p} {}
  constexpr ~holder() noexcept { reset(); }

  holder(const holder &) = delete;
  holder &operator=(const holder &) = delete;

  constexpr holder(holder &&o) : m_ptr(o.m_ptr) { o.m_ptr = nullptr; }
  constexpr holder &operator=(holder &&o) {
    if (m_ptr != o.m_ptr) {
      reset();
      m_ptr = o.m_ptr;
    }
    return *this;
  }

  [[nodiscard]] constexpr auto &operator*() noexcept { return m_ptr; }
  [[nodiscard]] constexpr const auto &operator*() const noexcept {
    return m_ptr;
  }
};

export class c_file : holder<FILE> {
public:
  explicit c_file(const char *name, const char *mode) noexcept
      : holder{fopen(name, mode)} {}
  using holder<FILE>::operator*;
};

export class c_memory : holder<void> {
public:
  explicit c_memory(unsigned count, unsigned size) noexcept
      : holder{calloc(count, size)} {}
  using holder<void>::operator*;
};

export template <typename Tp> class uptr {
  holder<Tp> m_holder;

public:
  constexpr uptr() noexcept = default;
  explicit constexpr uptr(Tp *ptr) : m_holder{ptr} {}

  template <typename... Args> static constexpr uptr<Tp> make(Args &&...args) {
    return uptr<Tp>{new Tp{traits::fwd<Args>(args)...}};
  }

  [[nodiscard]] constexpr operator bool() const noexcept {
    return *m_holder != nullptr;
  }

  [[nodiscard]] constexpr Tp &operator*() noexcept { return **m_holder; }
  [[nodiscard]] constexpr const Tp &operator*() const noexcept {
    return **m_holder;
  }
  [[nodiscard]] constexpr Tp *operator->() noexcept { return *m_holder; }
  [[nodiscard]] constexpr const Tp *operator->() const noexcept {
    return *m_holder;
  }
};
static_assert(*uptr<bool>::make(true));
static_assert(uptr<bool>::make(false) && !*uptr<bool>::make(false));
static_assert(!**uptr<uptr<bool>>::make(uptr<bool>::make(false)));
static_assert([] {
  uptr<bool> a = uptr<bool>::make(true);
  uptr<bool> b = traits::move(a);
  return !a && b && *b;
}());

template <typename Tp> class sptr_shr {
  unsigned m_count;
  holder<Tp> m_holder;

public:
  constexpr sptr_shr() = default;
  explicit constexpr sptr_shr(Tp *ptr) : m_count{1}, m_holder{ptr} {}

  constexpr void incr_use() noexcept { m_count++; }
  constexpr bool decr_use() noexcept {
    m_count--;
    return m_count == 0;
  }

  [[nodiscard]] constexpr Tp &operator*() noexcept { return **m_holder; }
  [[nodiscard]] constexpr const Tp &operator*() const noexcept {
    return **m_holder;
  }
  [[nodiscard]] constexpr Tp *operator->() noexcept { return *m_holder; }
  [[nodiscard]] constexpr const Tp *operator->() const noexcept {
    return *m_holder;
  }
};

export template <typename Tp> class sptr {
  sptr_shr<Tp> *m_shr;

  constexpr void reset() {
    if (m_shr && m_shr->decr_use())
      delete m_shr;
  }

public:
  constexpr sptr() noexcept = default;
  explicit constexpr sptr(Tp *ptr) : m_shr{new sptr_shr{ptr}} {}

  constexpr ~sptr() noexcept { reset(); }

  constexpr sptr(sptr &&o) : m_shr{o.m_shr} { o.m_shr = nullptr; }
  constexpr sptr &operator=(sptr &&o) {
    if (o.m_shr != m_shr) {
      m_shr = o.m_shr;
      o.m_shr = nullptr;
    }
    return *this;
  }

  constexpr sptr(const sptr &o) : m_shr{o.m_shr} { m_shr->incr_use(); }
  constexpr sptr &operator=(const sptr &o) {
    if (o.m_shr != m_shr) {
      o.m_shr->incr_use();
      reset();
      m_shr = o.m_shr;
    }
    return *this;
  }

  template <typename... Args> static constexpr sptr<Tp> make(Args &&...args) {
    return sptr<Tp>{new Tp{traits::fwd<Args>(args)...}};
  }

  [[nodiscard]] constexpr operator bool() const noexcept {
    return m_shr != nullptr;
  }

  [[nodiscard]] constexpr Tp &operator*() noexcept { return **m_shr; }
  [[nodiscard]] constexpr const Tp &operator*() const noexcept {
    return **m_shr;
  }
  [[nodiscard]] constexpr Tp *operator->() noexcept { return *m_shr; }
  [[nodiscard]] constexpr const Tp *operator->() const noexcept {
    return *m_shr;
  }
};
static_assert(*sptr<bool>::make(true));
static_assert(sptr<bool>::make(false) && !*sptr<bool>::make(false));
static_assert(!**sptr<sptr<bool>>::make(sptr<bool>::make(false)));
static_assert([] {
  sptr<bool> a = sptr<bool>::make(false);
  sptr<bool> b = traits::move(a);
  sptr<bool> c = b;
  b = c;
  {
    sptr<bool> d = c;
    *d = true;
  }
  return !a && b && c && *b && *c;
}());
} // namespace hai
