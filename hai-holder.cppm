export module hai:holder;
import traits;

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
  constexpr value_holder() = default;
  explicit constexpr value_holder(Tp p) : m_ptr{p} {}
  constexpr ~value_holder() { reset(); }

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

template<typename T, void (*Del)(T)>
struct fn_deleter { constexpr void operator()(T t) { Del(t); } };
export template<typename T, void (*Del)(T)>
struct value : value_holder<T, fn_deleter<T, Del>> {
  using value_holder<T, fn_deleter<T, Del>>::value_holder;
};
static_assert([] {
  static constexpr const auto del = [](int * p) { delete p; };
  value<int *, del> p { new int };
  return true;
}());

template <typename Tp> struct deleter<Tp *> {
  constexpr void operator()(Tp *f) const { delete f; }
};
export template <typename Tp, typename Del = deleter<Tp *>>
struct holder : value_holder<Tp *, Del> {
  using value_holder<Tp *, Del>::value_holder;
  using value_holder<Tp *, Del>::operator*;
};

template <typename Tp> struct deleter<Tp[]> {
  constexpr void operator()(Tp *f) const { delete[] f; }
};
export template <typename Tp> struct holder<Tp[]> : holder<Tp, deleter<Tp[]>> {
  using holder<Tp, deleter<Tp[]>>::holder;
  using holder<Tp, deleter<Tp[]>>::operator*;

  static constexpr auto make(int size) { return holder<Tp[]>{new Tp[size]{}}; };
};
static_assert([] {
  auto h = holder<bool[]>::make(3);
  (*h)[1] = true;
  return (*h)[1];
}());
} // namespace hai
