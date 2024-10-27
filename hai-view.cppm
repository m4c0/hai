export module hai:view;
import traits;
import :array;
import :sptr;

namespace hai {
  export template<typename T> class view {
    sptr<array<T>> m_data {};

  public:
    constexpr view() = default;

    constexpr view(traits::same_as<T> auto &&... args)
      : m_data { new array<T>{ array<T>::make(args...) } } {}

    [[nodiscard]] constexpr auto begin() const { return m_data->begin(); }
    [[nodiscard]] constexpr auto end() const { return m_data->end(); }
    [[nodiscard]] constexpr auto size() const { return m_data->size(); }
    [[nodiscard]] constexpr T operator[](unsigned idx) const { return (*m_data)[idx]; }
  };
}

static_assert([] {
  static constexpr auto fail = [] -> bool { throw 0; };

  hai::view<int> v { 1, 2, 3 };
  (v.size() == 3) || fail();
  (v[0] == 1) || fail();
  (v[1] == 2) || fail();
  (v[2] == 3) || fail();

  auto vv = v;

  auto n = 1;
  for (auto & o : vv) (o == n++) || fail();

  return true;
}());
