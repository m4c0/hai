export module hai:array;
import :holder;

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
  for (auto &i : test) {
    i = 1;
  }
  test[2] = 2;

  unsigned sum = 0;
  const auto &t = test;
  for (auto &i : test) {
    sum += i;
  }
  return sum == 4;
}());
