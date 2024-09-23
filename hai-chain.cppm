export module hai:chain;

namespace hai {
  template <typename T> class chain {

  public:
    explicit constexpr chain(unsigned bucket_size) {}

    constexpr void push_back(const T & t) {}
    constexpr void push_back(T && t) {}

    [[nodiscard]] constexpr unsigned size() const { return 0; }
  };
} // namespace hai

static_assert([] {
  hai::chain<int> c { 4 };
  for (auto i = 0; i < 16; i++) c.push_back(i * i);

  if (c.size() != 16) throw 0;

  unsigned i = 0;
  for (auto n : c) {
    if (n != i * i) throw 0;
    i++;
  }

  return true;
}());
