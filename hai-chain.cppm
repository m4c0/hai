export module hai:chain;
import :uptr;
import :varray;
import traits;

namespace hai {
  export template <typename T> class chain {
    hai::varray<T> m_data;
    unsigned m_size {};
    hai::uptr<chain<T>> m_next {};
    chain<T> * m_last { this };

    struct mit {
      chain<T> * m_h;
      unsigned m_pos {};

      [[nodiscard]] constexpr bool operator==(const mit & o) const { return m_h == o.m_h && m_pos == o.m_pos; }

      [[nodiscard]] constexpr mit & operator++() {
        m_pos++;
        if (m_pos < m_h->m_data.size()) return *this;
        *this = {};
        return *this;
      }

      [[nodiscard]] constexpr T & operator*() { return m_h->m_data[m_pos]; }
    };
    struct cit {
      const chain<T> * m_h;
      unsigned m_pos {};

      [[nodiscard]] constexpr bool operator==(const cit & o) const { return m_h == o.m_h && m_pos == o.m_pos; }

      [[nodiscard]] constexpr cit & operator++() {
        m_pos++;
        if (m_pos < m_h->m_data.size()) return *this;
        *this = {};
        return *this;
      }

      [[nodiscard]] constexpr const T & operator*() const { return m_h->m_data[m_pos]; }
    };

    constexpr void find_ptr() {
      if (m_last->m_data.capacity() == m_last->m_data.size()) {
        m_last->m_next.reset(new chain<T> { m_data.capacity() });
        m_last = &*m_next;
      }
    }

  public:
    constexpr chain() : chain { 16 } {}
    explicit constexpr chain(unsigned bucket_size) : m_data { bucket_size } {}

    constexpr void push_back(const T & t) {
      find_ptr();
      m_last->m_data.push_back(t);
      m_size++;
    }
    constexpr void push_back(T && t) {
      find_ptr();
      m_last->m_data.push_back(traits::move(t));
      m_size++;
    }

    [[nodiscard]] constexpr auto begin() const { return cit { this }; }
    [[nodiscard]] constexpr auto end() const { return cit { nullptr }; }

    [[nodiscard]] constexpr auto begin() { return mit { this }; }
    [[nodiscard]] constexpr auto end() { return mit { nullptr }; }

    [[nodiscard]] constexpr unsigned size() const { return m_size; }
  };
} // namespace hai

static_assert([] {
  constexpr const auto el_count = 5;

  hai::chain<int> c { 4 };
  for (auto i = 0; i < el_count; i++) c.push_back(i * i);

  if (c.size() != el_count) throw 0;
  if (c.begin() == c.end()) throw 0;

  unsigned i = 0;
  for (auto n : c) {
    if (n != i * i) throw 0;
    i++;
  }

  return true;
}());
