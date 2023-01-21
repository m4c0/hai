export module hai:cstr;
import :holder;

namespace hai {
// cstr stores a null-terminated string, but it exposes only the characters via
// normal access.
export class cstr {
  holder<char[]> m_buf{};
  unsigned m_size{};

public:
  constexpr cstr() = default;
  explicit constexpr cstr(unsigned sz)
      : m_buf{new char[sz + 1]{}}, m_size{sz} {}

  [[nodiscard]] constexpr char *data() noexcept { return *m_buf; }
  [[nodiscard]] constexpr const char *data() const noexcept { return *m_buf; }
  [[nodiscard]] constexpr auto size() const noexcept { return m_size; }

  [[nodiscard]] constexpr auto begin() const noexcept { return *m_buf; }
  [[nodiscard]] constexpr auto end() const noexcept { return *m_buf + m_size; }
};

static_assert([] {
  cstr c{4};
  for (char &cc : c) {
    cc = '0';
  }
  c.data()[0] = 'd';
  return c.data()[0] == 'd' && c.data()[1] == '0' && c.data()[4] == 0;
}());
} // namespace hai
