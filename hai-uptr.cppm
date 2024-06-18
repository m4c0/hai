export module hai:uptr;
import :holder;
import traits;

namespace hai {
export template <typename Tp> class uptr {
  holder<Tp> m_holder;

public:
  constexpr uptr() = default;
  explicit constexpr uptr(Tp *ptr) : m_holder{ptr} {}

  template <typename... Args> static constexpr uptr<Tp> make(Args &&...args) {
    return uptr<Tp>{new Tp{traits::fwd<Args>(args)...}};
  }

  constexpr void reset(Tp *p) { m_holder.reset(p); }
  [[nodiscard]] constexpr Tp *release() { return m_holder.release(); }

  [[nodiscard]] constexpr operator bool() const { return *m_holder != nullptr; }

  [[nodiscard]] constexpr Tp &operator*() { return **m_holder; }
  [[nodiscard]] constexpr const Tp &operator*() const { return **m_holder; }
  [[nodiscard]] constexpr Tp *operator->() { return *m_holder; }
  [[nodiscard]] constexpr const Tp *operator->() const { return *m_holder; }
};
static_assert(*uptr<bool>::make(true));
static_assert(uptr<bool>::make(false) && !*uptr<bool>::make(false));
static_assert(!**uptr<uptr<bool>>::make(uptr<bool>::make(false)));
static_assert([] {
  uptr<bool> a = uptr<bool>::make(true);
  uptr<bool> b = traits::move(a);
  return !a && b && *b;
}());
static_assert([] {
  uptr<bool> a = uptr<bool>::make(true);
  uptr<bool> b{a.release()};
  return !a && b && *b;
}());
static_assert([] {
  uptr<bool> a = uptr<bool>::make(true);
  a = traits::move(a);
  return a && *a;
}());
static_assert([] {
  uptr<bool> a = uptr<bool>::make(false);
  auto *b = new bool{true};
  a.reset(b);
  a.reset(b);
  return a && *a;
}());
} // namespace hai
