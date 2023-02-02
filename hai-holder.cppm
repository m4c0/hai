export module hai:holder;
import traits;

// This is only needed until clang fixes a linking bug. Without these, anything
// based on hai fails because of missing "hai::operator X"
extern "C" void *malloc(traits::size_t);
extern "C" void free(void *);
void *operator new(traits::size_t count) { return malloc(count); }
void *operator new[](traits::size_t count) { return malloc(count); }
void operator delete(void *ptr) noexcept { return free(ptr); }
void operator delete[](void *ptr) noexcept { return free(ptr); }

namespace hai {
template <typename Tp> struct deleter;
export template <typename Tp, typename Del = deleter<Tp>> class value_holder {
  Tp m_ptr{};

  constexpr void reset() {
    if (m_ptr)
      Del{}(m_ptr);

    m_ptr = {};
  }

public:
  constexpr value_holder() noexcept = default;
  explicit constexpr value_holder(Tp p) noexcept : m_ptr{p} {}
  constexpr ~value_holder() noexcept { reset(); }

  value_holder(const value_holder &) = delete;
  value_holder &operator=(const value_holder &) = delete;

  constexpr value_holder(value_holder &&o) : m_ptr(o.release()) {}
  constexpr value_holder &operator=(value_holder &&o) {
    if (m_ptr != o.m_ptr) {
      reset();
      m_ptr = o.release();
    }
    return *this;
  }

  [[nodiscard]] constexpr auto release() noexcept {
    auto res = m_ptr;
    m_ptr = nullptr;
    return res;
  }

  [[nodiscard]] constexpr auto &operator*() noexcept { return m_ptr; }
  [[nodiscard]] constexpr const auto &operator*() const noexcept {
    return m_ptr;
  }
};

template <typename Tp> struct deleter<Tp *> {
  constexpr void operator()(Tp *f) const noexcept { delete f; }
};
export template <typename Tp, typename Del = deleter<Tp *>>
struct holder : value_holder<Tp *, Del> {
  using value_holder<Tp *, Del>::value_holder;
  using value_holder<Tp *, Del>::operator*;
};

template <typename Tp> struct deleter<Tp[]> {
  constexpr void operator()(Tp *f) const noexcept { delete[] f; }
};
export template <typename Tp> struct holder<Tp[]> : holder<Tp, deleter<Tp[]>> {
  using holder<Tp, deleter<Tp[]>>::holder;
  using holder<Tp, deleter<Tp[]>>::operator*;

  static auto make(int size) { return holder<Tp[]>{new Tp[size]}; };
};
static_assert([] {
  auto h = holder<bool[]>::make(3);
  (*h)[1] = true;
  return (*h)[1];
});
} // namespace hai
