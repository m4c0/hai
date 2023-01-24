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
template <typename Tp> struct deleter {
  constexpr void operator()(Tp *f) const noexcept { delete f; }
};
export template <typename Tp, typename Del = deleter<Tp>> class holder {
  Tp *m_ptr;

  constexpr void reset() {
    if (m_ptr != nullptr)
      Del{}(m_ptr);

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

template <typename Tp> struct deleter<Tp[]> {
  constexpr void operator()(Tp *f) const noexcept { delete[] f; }
};
export template <typename Tp> struct holder<Tp[]> : holder<Tp, deleter<Tp[]>> {
  using holder<Tp, deleter<Tp[]>>::holder;
  using holder<Tp, deleter<Tp[]>>::operator*;
};
} // namespace hai
