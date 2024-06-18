export module hai:sptr;
import :holder;
import traits;

namespace hai {
template <typename Tp> class sptr_shr {
  unsigned m_count;
  holder<Tp> m_holder;

public:
  constexpr sptr_shr() = default;
  explicit constexpr sptr_shr(Tp *ptr) : m_count{1}, m_holder{ptr} {}

  constexpr void incr_use() { m_count++; }
  constexpr bool decr_use() {
    m_count--;
    return m_count == 0;
  }

  [[nodiscard]] constexpr Tp *operator*() { return *m_holder; }
  [[nodiscard]] constexpr const Tp *operator*() const { return *m_holder; }
};

export template <typename Tp> class sptr {
  sptr_shr<Tp> *m_shr{};

  constexpr void reset() {
    if (m_shr && m_shr->decr_use())
      delete m_shr;
  }

public:
  constexpr sptr() = default;
  explicit constexpr sptr(Tp *ptr) : m_shr{new sptr_shr{ptr}} {}

  constexpr ~sptr() { reset(); }

  constexpr sptr(sptr &&o) : m_shr{o.m_shr} { o.m_shr = nullptr; }
  constexpr sptr &operator=(sptr &&o) {
    if (o.m_shr != m_shr) {
      m_shr = o.m_shr;
      o.m_shr = nullptr;
    }
    return *this;
  }

  constexpr sptr(const sptr &o) : m_shr{o.m_shr} {
    if (m_shr != nullptr)
      m_shr->incr_use();
  }
  constexpr sptr &operator=(const sptr &o) {
    if (o.m_shr != m_shr) {
      if (o.m_shr != nullptr)
        o.m_shr->incr_use();
      reset();
      m_shr = o.m_shr;
    }
    return *this;
  }

  template <typename... Args> static constexpr sptr<Tp> make(Args &&...args) {
    return sptr<Tp>{new Tp{traits::fwd<Args>(args)...}};
  }

  [[nodiscard]] constexpr operator bool() const { return m_shr != nullptr; }

  [[nodiscard]] constexpr Tp &operator*() { return ***m_shr; }
  [[nodiscard]] constexpr const Tp &operator*() const { return ***m_shr; }
  [[nodiscard]] constexpr Tp *operator->() { return **m_shr; }
  [[nodiscard]] constexpr const Tp *operator->() const { return **m_shr; }
};
static_assert(*sptr<bool>::make(true));
static_assert(sptr<bool>::make(false) && !*sptr<bool>::make(false));
static_assert(!**sptr<sptr<bool>>::make(sptr<bool>::make(false)));
static_assert([] {
  sptr<bool> a = sptr<bool>::make(false);
  sptr<bool> b = traits::move(a);
  sptr<bool> c{b};
  b = c;
  {
    sptr<bool> d = c;
    *d = true;
  }
  return !a && b && c && *b && *c;
}());
static_assert([] {
  sptr<bool> a;
  sptr<bool> b{a};
  sptr<bool> c = b;
  return true;
}());
static_assert([] {
  struct test {
    int t;
  };
  sptr<test>::make()->t = 0;
  return true;
}());
} // namespace hai
