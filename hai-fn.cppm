export module hai:fn;
import :sptr;
import no;
export import traits;

namespace hai {
export template <typename Ret, typename... Args> class fn {
  struct hld {
    constexpr virtual ~hld() {}
    constexpr virtual Ret operator()(Args &&... args) = 0;
  };
  template<typename T>
  struct shrd : hld {
    T ptr;

    constexpr shrd(auto && p) : ptr{traits::move(p)} {}
    constexpr Ret operator()(Args &&... args) override {
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

  template<typename T>
  constexpr fn(T * t, Ret (T::*m)(Args...)) : fn([t, m](Args &&... args) -> Ret { return (t->*m)(args...); }) {}
  template<typename T>
  constexpr fn(const T * t, Ret (T::*m)(Args...) const) : fn([t, m](Args &&... args) -> Ret { return (t->*m)(args...); }) {}

  constexpr explicit operator bool() const { return m_data; }

  constexpr Ret operator()(auto &&...args) { return (*m_data)(traits::fwd<Args>(args)...); };
};
template<typename T, typename Ret, typename... Args> fn(T * t, Ret (T::*m)(Args...)) -> fn<Ret, Args...>;
template<typename T, typename Ret, typename... Args> fn(T * t, Ret (T::*m)(Args...) const) -> fn<Ret, Args...>;
} // namespace hai

static_assert([]{
  int c = 1;
  hai::fn<int, int, int> fn{[=](auto a, auto b) { return a + b + c; }};
  return fn(2, 3) == 6;
}());

static_assert([]{
  struct t {
    static constexpr bool test() { return true; }
  };
  hai::fn<bool> fn{t::test};
  hai::fn<bool> fn2{fn};
  hai::fn<bool> fn3{traits::move(fn2)};
  return fn() && fn3();
}());

static_assert([] {
  struct t {
    constexpr bool test() { return true; }
  } tt;
  
  hai::fn<bool> fn { &tt, &t::test };
  return fn();
}());

static_assert([] {
  struct x : no::copy { constexpr bool ok() { return true; } };
  hai::fn<bool> fn{[xx = x()] mutable { return xx.ok(); }};
  return fn();
}());

static_assert([] {
  struct x_t { constexpr bool ok() { return true; } } x;
  auto fn = hai::fn { &x, &x_t::ok };
  return fn();
}());
static_assert([] {
  struct x_t { constexpr bool ok() const { return true; } } const x;
  auto fn = hai::fn { &x, &x_t::ok };
  return fn();
}());
