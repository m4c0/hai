export module hai:cstr;
import :holder;
import sv;

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

  explicit constexpr cstr(sv sv) : cstr { static_cast<unsigned>(sv.size()) } {
    auto d = *m_buf;
    for (auto c : sv) *d++ = c;
  }

  [[nodiscard]] constexpr char *data() { return *m_buf; }
  [[nodiscard]] constexpr const char *data() const { return *m_buf; }
  [[nodiscard]] constexpr auto size() const { return m_size; }

  [[nodiscard]] constexpr auto begin() const { return *m_buf; }
  [[nodiscard]] constexpr auto end() const { return *m_buf + m_size; }
};

static_assert([] {
  cstr c{4};
  for (char &cc : c) {
    cc = '0';
  }
  c.data()[0] = 'd';
  return c.data()[0] == 'd' && c.data()[1] == '0' && c.data()[4] == 0;
}());
static_assert([] {
  cstr c { "test"_sv };
  return sv { c } == "test"_sv;
}());
} // namespace hai
