export module hai:varray;
import :array;
import traits;

namespace hai {
export template <typename Tp> class varray : public array<Tp> {
  unsigned m_count{};

public:
  constexpr varray() noexcept = default;
  constexpr explicit varray(unsigned capacity) : array<Tp>{capacity} {}

  [[nodiscard]] constexpr auto capacity() const noexcept {
    return array<Tp>::size();
  }
  [[nodiscard]] constexpr auto size() const noexcept { return m_count; }

  [[nodiscard]] constexpr auto *end() noexcept {
    return this->begin() + m_count;
  }
  [[nodiscard]] constexpr const auto *end() const noexcept {
    return this->begin() + m_count;
  }

  constexpr void push_back(auto &&v) noexcept {
    // TODO: throw?
    if (m_count == capacity())
      return;

    (*this)[m_count++] = traits::move(v);
  }
  constexpr Tp pop_back() noexcept {
    if (m_count > 0)
      --m_count;
    return traits::move((*this)[m_count]);
  }
};
} // namespace hai

static_assert([] {
  hai::varray<int> data{4};
  if (data.size() != 0)
    return false;

  data.push_back(45);
  if (data.size() != 1)
    return false;
  if (data[0] != 45)
    return false;

  data.push_back(33);
  if (data.size() != 2)
    return false;
  if (data[1] != 33)
    return false;

  if (data.pop_back() != 33)
    return false;
  return data.size() == 1;
}());
static_assert([] {
  struct unmov {
    constexpr unmov() = default;
    constexpr unmov(const unmov &) = delete;
    constexpr unmov(unmov &&) = default;
    constexpr unmov &operator=(const unmov &) = delete;
    constexpr unmov &operator=(unmov &&) = default;
  };
  hai::varray<unmov> data{4};
  data.push_back(unmov{});
  auto p = data.pop_back();
  return true;
}());
