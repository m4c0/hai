module;
// This is only required when compiling on MacOSX, but it fails on Android (for
// reasons)
#ifdef __APPLE__
#include <new>
#endif

export module hai:holder;

namespace hai {
template <typename Tp> struct deleter {
  constexpr void operator()(Tp *f) const noexcept { delete f; }
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
} // namespace hai
