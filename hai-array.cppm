export module hai:array;
import :holder;
import traits;

namespace hai {
export template <typename Tp> class array {
  holder<Tp[]> m_data{};
  unsigned m_size{};

public:
  constexpr array() = default;
  constexpr explicit array(unsigned size)
      : m_data{decltype(m_data)::make(size)}, m_size{size} {}
  template<unsigned N>
  constexpr array(const Tp (&data)[N]) : array(N) {
    for (auto i = 0; i < N; i++) (*m_data)[i] = data[i];
  }

  [[nodiscard]] constexpr auto data(this auto && self) { return *self.m_data; }
  [[nodiscard]] constexpr auto size() const { return m_size; }

  [[nodiscard]] constexpr auto & operator[](this auto && self, unsigned idx) {
    return self.data()[idx];
  }

  [[nodiscard]] constexpr auto * begin(this auto && self) { return &self[0]; }
  [[nodiscard]] constexpr auto * end(this auto && self) { return &self[self.m_size]; }

  constexpr void set_capacity(unsigned qty) {
    if (qty < m_size)
      return;

    auto old = traits::move(m_data);
    m_data = decltype(m_data)::make(qty);
    for (auto i = 0; i < m_size; i++) {
      (*m_data)[i] = traits::move((*old)[i]);
    }
    m_size = qty;
  }
  constexpr void add_capacity(unsigned qty) { set_capacity(qty + m_size); }

  [[nodiscard]] static constexpr auto make(auto &&... args) {
    array<Tp> res{sizeof...(args)};
    unsigned i = 0;
    ((res[i++] = args), ...);
    return res;
  }
};
} // namespace hai

static_assert([] {
  hai::array<int> test{3};
  if (test.size() != 3)
    return false;

  for (auto &i : test) {
    i = 1;
  }
  test[2] = 2;

  test.add_capacity(1);
  if (test.size() != 4)
    return false;
  if (test[3] != 0)
    return false;

  unsigned sum = 0;
  for (auto &i : test) {
    sum += i;
  }

  return sum == 4;
}());
static_assert([] {
  auto arr = hai::array<int>::make(7, 4, 1);
  if (arr.size() != 3)
    throw 0;
  if (arr[0] != 7)
    throw 0;
  if (arr[1] != 4)
    throw 0;
  if (arr[2] != 1)
    throw 0;
  return true;
}());
static_assert([] {
  hai::array<int> arr {{ 7, 4, 1 }};
  if (arr.size() != 3) throw 0;
  if (arr[0] != 7) throw 0;
  if (arr[1] != 4) throw 0;
  if (arr[2] != 1) throw 0;
  return true;
}());
