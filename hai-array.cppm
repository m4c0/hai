export module hai:array;
import :holder;
import traits;

namespace hai {
export template <typename Tp> class array {
  holder<Tp[]> m_data{};
  unsigned m_size;

public:
  constexpr array() noexcept = default;
  constexpr explicit array(unsigned size)
      : m_data{decltype(m_data)::make(size)}, m_size{size} {}

  [[nodiscard]] constexpr auto &operator[](unsigned idx) noexcept {
    return (*m_data)[idx];
  }
  [[nodiscard]] constexpr const auto &operator[](unsigned idx) const noexcept {
    return (*m_data)[idx];
  }

  [[nodiscard]] constexpr auto size() const noexcept { return m_size; }

  constexpr void add_capacity(unsigned qty) {
    auto old = traits::move(m_data);
    m_data = decltype(m_data)::make(qty + m_size);
    for (auto i = 0; i < m_size; i++) {
      (*m_data)[i] = (*old)[i];
    }
    m_size += qty;
  }

  [[nodiscard]] constexpr auto *begin() noexcept { return &(*m_data)[0]; }
  [[nodiscard]] constexpr const auto *begin() const noexcept {
    return &(*m_data)[0];
  }

  [[nodiscard]] constexpr auto *end() noexcept { return &(*m_data)[m_size]; }
  [[nodiscard]] constexpr const auto *end() const noexcept {
    return &(*m_data)[m_size];
  }
};
} // namespace hai

static_assert([] {
  hai::array<int> test{3};
  if (test.size() != 3)
    throw 0;

  for (auto &i : test) {
    i = 1;
  }
  test[2] = 2;

  test.add_capacity(1);
  if (test.size() != 4)
    throw 0;
  if (test[3] != 0)
    throw 0;

  unsigned sum = 0;
  const auto &t = test;
  for (auto &i : test) {
    sum += i;
  }

  return sum == 4;
}());
