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
  constexpr fn(traits::is_callable_r<Ret, Args...> auto &&fn) {
    using T = traits::remove_ref_t<decltype(fn)>;
    m_data = hai::sptr<hld>{new shrd<T>{traits::fwd<T>(fn)}};
  }
  constexpr fn(Ret (*fn)(Args...)) {
    using T = Ret (*)(Args...);
    m_data = hai::sptr<hld>{new shrd<T>{traits::fwd<T>(fn)}};
  }

  constexpr explicit operator bool() const { return m_data; }

  constexpr Ret operator()(Args &&...args) const { return (*m_data)(traits::fwd<Args>(args)...); };
};
} // namespace hai

static_assert([]{
  int c = 1;
  hai::fn<int, int, int> fn{[=](auto a, auto b) { return a + b + c; }};
  return fn(2, 3) == 6;
}());
