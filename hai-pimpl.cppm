export module hai:pimpl;

namespace hai {
/// Manages the lifecycle of a resource (memory pointer, handlers, etc) in a way
/// its "owner" doesn't need to know about the dtor.
///
/// It is similar to "value_holder" used in a pattern like this:
///
/// struct deleter {
///   void operator(*)(type);
/// }
/// hai::value_holder<type, deleter> value;
///
/// This also enables full-pimpl support if "type" is a pointer to a forwarded
/// class.
export template <typename Tp> class pimpl {
  using deleter_t = void (*)(Tp);

  Tp m_ptr{};
  deleter_t m_del{};

  constexpr void reset() {
    if (m_ptr)
      m_del(m_ptr);

    m_ptr = {};
  }

public:
  constexpr pimpl() = default;
  constexpr pimpl(Tp p, deleter_t d) : m_ptr{p}, m_del{d} {}
  constexpr ~pimpl() { reset(); }

  pimpl(const pimpl &) = delete;
  pimpl &operator=(const pimpl &) = delete;

  constexpr pimpl(pimpl &&o) : m_ptr(o.release()) {}
  constexpr pimpl &operator=(pimpl &&o) {
    if (m_ptr != o.m_ptr) {
      reset();
      m_ptr = o.release();
    }
    return *this;
  }

  constexpr void reset(Tp p) {
    if (m_ptr != p) {
      reset();
      m_ptr = p;
    }
  }
  [[nodiscard]] constexpr auto release() {
    auto res = m_ptr;
    m_ptr = {};
    return res;
  }

  [[nodiscard]] constexpr auto &operator*() { return m_ptr; }
  [[nodiscard]] constexpr const auto &operator*() const { return m_ptr; }
};
} // namespace hai

static_assert([] {
  constexpr const auto deleter = [](int *n) { delete n; };

  hai::pimpl<int *> i{new int{}, deleter};
  return true;
}());
