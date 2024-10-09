export module hai:chain;
import :uptr;
import :varray;
import traits;

namespace hai::chain_impl {
  template <typename T> struct node {
    struct overflow {};

    hai::varray<T> m_data;
    hai::uptr<node<T>> m_next {};

  public:
    constexpr node() : node { 16 } {}
    explicit constexpr node(unsigned bucket_size) : m_data { bucket_size } {}

    [[nodiscard]] constexpr const T & seek(unsigned i) const {
      if (i < m_data.size()) return m_data[i];
      if (!m_next) throw overflow {};
      return m_next->seek(i - m_data.size());
    }
    [[nodiscard]] constexpr T & seek(unsigned i) {
      if (i < m_data.size()) return m_data[i];
      if (!m_next) throw overflow {};
      return m_next->seek(i - m_data.size());
    }
  };

  template <typename T> struct cit {
    const node<T> * m_h;
    unsigned m_pos {};

    [[nodiscard]] constexpr bool operator==(const cit & o) const { return m_h == o.m_h && m_pos == o.m_pos; }

    [[nodiscard]] constexpr cit & operator++() {
      m_pos++;
      if (m_pos < m_h->m_data.size()) return *this;
      *this = { &*m_h->m_next, 0 };
      return *this;
    }

    [[nodiscard]] constexpr const T & operator*() { return m_h->m_data[m_pos]; }
  };
  template <typename T> struct mit {
    node<T> * m_h;
    unsigned m_pos {};

    [[nodiscard]] constexpr bool operator==(const mit & o) const { return m_h == o.m_h && m_pos == o.m_pos; }

    [[nodiscard]] constexpr mit & operator++() {
      m_pos++;
      if (m_pos < m_h->m_data.size()) return *this;
      *this = { &*m_h->m_next, 0 };
      return *this;
    }

    [[nodiscard]] constexpr T & operator*() { return m_h->m_data[m_pos]; }
  };
  static_assert([] {
    node<int> a { 4 };
    a.m_data.push_back(20);
    a.m_data.push_back(30);

    auto b = new node<int> { 4 };
    a.m_next.reset(b);
    b->m_data.push_back(40);

    mit<int> it { &a, 0 };
    if (*it != 20) throw 0;
    if (*++it != 30) throw 0;
    if (*++it != 40) throw 0;
    if (++it != mit<int> {}) throw 0;

    return true;
  }());
} // namespace hai::chain_impl
namespace hai {
  export template <typename T> class chain {
    hai::uptr<chain_impl::node<T>> m_first;
    chain_impl::node<T> * m_last;
    unsigned m_size {};

    constexpr void find_ptr() {
      if (m_last->m_data.capacity() == m_last->m_data.size()) {
        auto n = new chain_impl::node<T> { m_first->m_data.capacity() };
        m_last->m_next.reset(n);
        m_last = n;
      }
    }

  public:
    constexpr chain() : chain { 16 } {}
    constexpr chain(unsigned node_size) : m_first { new chain_impl::node<T> { node_size } }, m_last { &*m_first } {}

    [[nodiscard]] constexpr auto begin() { return chain_impl::mit<T> { m_size == 0 ? nullptr : &*m_first, 0 }; }
    [[nodiscard]] constexpr auto end() { return chain_impl::mit<T> {}; }

    [[nodiscard]] constexpr auto begin() const { return chain_impl::cit<T> { m_size == 0 ? nullptr : &*m_first, 0 }; }
    [[nodiscard]] constexpr auto end() const { return chain_impl::cit<T> {}; }

    [[nodiscard]] constexpr auto size() const { return m_size; }

    [[nodiscard]] constexpr auto & seek(unsigned n) const { return m_first->seek(n); }
    [[nodiscard]] constexpr auto & seek(unsigned n) { return m_first->seek(n); }

    constexpr void push_back(const T & t) {
      find_ptr();
      m_last->m_data.push_back(t);
      this->m_size++;
    }
    constexpr void push_back(T && t) {
      find_ptr();
      m_last->m_data.push_back(traits::move(t));
      this->m_size++;
    }
  };
} // namespace hai

static_assert([] {
  constexpr const auto el_count = 16;

  hai::chain<int> c { 4 };
  if (c.begin() != c.end()) throw 0;

  for (auto i = 0; i < el_count; i++) c.push_back(i * i);

  if (c.size() != el_count) throw 0;
  if (c.begin() == c.end()) throw 0;

  unsigned i = 0;
  for (auto n : c) {
    if (i >= el_count) throw 0;
    if (n != i * i) throw 0;
    i++;
  }
  if (i != el_count) throw 0;

  return c.seek(5) == 25;
}());
static_assert([] {
  hai::chain<int> c {};
  hai::chain<int> b = traits::move(c);

  b.push_back(100);

  return b.seek(0) == 100;
}());
static_assert([] {
  hai::chain<int> c {};
  c.push_back(100);

  const auto & b = c;
  for (auto _ : b) {
    // just testing if we can const-iterate
  }

  return true;
}());
