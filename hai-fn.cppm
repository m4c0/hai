export module hai:fn;
import :sptr;
export import traits;

namespace hai {
export template <typename Ret, typename... Args> class fn {
  struct hld {
    constexpr virtual ~hld() {}
    constexpr virtual Ret operator()(Args &&... args) const = 0;
  };
  template<typename T>
  struct shrd : hld {
    T ptr;

    constexpr shrd(auto p) : ptr{p} {}
    constexpr Ret operator()(Args &&... args) const override {
      return ptr(traits::fwd<Args>(args)...);
    }
  };
  hai::sptr<hld> m_data{};

public:
  constexpr fn() = default;
  constexpr fn(const fn<Ret, Args...> &) = default;
  constexpr fn(fn<Ret, Args...> &&) = default;
  constexpr fn & operator=(const fn<Ret, Args...> &) = default;
  constexpr fn & operator=(fn<Ret, Args...> &&) = default;

  template<typename T>
    requires traits::is_callable_r<T, Ret, Args...> && (!traits::same_as<traits::remove_ref_t<T>, fn<Ret, Args...>>)
  constexpr fn(T &&fn) {
    using TT = traits::remove_ref_t<decltype(fn)>;
    m_data = hai::sptr<hld>{new shrd<TT>{traits::fwd<T>(fn)}};
  }
  constexpr fn(Ret (*fn)(Args...)) {
    using T = Ret (*)(Args...);
    m_data = hai::sptr<hld>{new shrd<T>{traits::fwd<T>(fn)}};
  }

  constexpr explicit operator bool() const { return m_data; }

  constexpr Ret operator()(auto &&...args) const { return (*m_data)(traits::fwd<Args>(args)...); };
};
} // namespace hai

static_assert([]{
  int c = 1;
  hai::fn<int, int, int> fn{[=](auto a, auto b) { return a + b + c; }};
  return fn(2, 3) == 6;
}());

static constexpr bool test() { return true; }
static_assert([]{
  const hai::fn<bool> fn{test};
  hai::fn<bool> fn2{fn};
  hai::fn<bool> fn3{traits::move(fn2)};
  return fn() && fn3();
}());
