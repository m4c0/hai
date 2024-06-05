export module hai:varray;
import :array;
import traits;

namespace hai {
export template <typename Tp> class varray : public array<Tp> {
  static constexpr const auto initial_auto_capacity = 16;

  unsigned m_count{};

public:
  constexpr varray() noexcept = default;
  constexpr explicit varray(unsigned capacity) : array<Tp>{capacity} {}

  [[nodiscard]] constexpr auto capacity() const noexcept {
    return array<Tp>::size();
  }
  [[nodiscard]] constexpr auto size() const noexcept { return m_count; }

  [[nodiscard]] constexpr auto has_capacity() const noexcept {
    return capacity() > size();
  }

  [[nodiscard]] constexpr auto *end() noexcept {
    return this->begin() + m_count;
  }
  [[nodiscard]] constexpr const auto *end() const noexcept {
    return this->begin() + m_count;
  }

  [[nodiscard]] constexpr auto back() const noexcept {
    return (*this)[m_count - 1];
  }

  constexpr void truncate(unsigned c) noexcept {
    if (c >= m_count)
      return;

    m_count = c;
  }
  constexpr void expand(unsigned c) noexcept {
    if (c <= m_count || c > capacity())
      return;

    m_count = c;
  }

  constexpr void push_back_doubling(auto &&v) noexcept {
    if (m_count == capacity()) {
      this->add_capacity(size() == 0 ? initial_auto_capacity : size());
    }

    (*this)[m_count++] = traits::move(v);
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

  if (data.back() != 33)
    return false;
  if (data.pop_back() != 33)
    return false;
  return data.size() == 1;
}());
static_assert([] {
  hai::varray<int> data{1};
  data.push_back_doubling(10);
  data.push_back_doubling(20);
  data.push_back_doubling(30);
  if (data[0] != 10)
    return false;
  if (data[1] != 20)
    return false;
  if (data[2] != 30)
    return false;
  return data.size() == 3 && data.capacity() == 4;
}());
static_assert([] {
  hai::varray<int> data{};
  data.push_back_doubling(0);
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
  [[maybe_unused]] auto p = data.pop_back();
  return true;
}());
