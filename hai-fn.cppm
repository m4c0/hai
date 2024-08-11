export module hai:fn;
import :sptr;
export import traits;

namespace hai {
export template <typename Ret, typename... Args> class fn {
  struct shrd {
    void *ptr = nullptr;
    Ret (*wrap)(void *, Args &&...) = nullptr;

    ~shrd() { ::operator delete(ptr); }
  };
  hai::sptr<shrd> m_data{};

public:
  constexpr fn() = default;
  fn(traits::is_callable_r<Ret, Args...> auto &&fn) {
    using T = traits::remove_ref_t<decltype(fn)>;
    m_data = hai::sptr<shrd>{new shrd{}};
    m_data->ptr = new T{ traits::fwd<T>(fn) };
    m_data->wrap = [](void *ptr, Args &&...args) -> Ret { return (*reinterpret_cast<T *>(ptr))(traits::fwd<Args>(args)...); };
  }

  constexpr explicit operator bool() const { return m_data; }

  Ret operator()(Args &&...args) const { return m_data->wrap(m_data->ptr, traits::fwd<Args>(args)...); };
};
} // namespace hai
