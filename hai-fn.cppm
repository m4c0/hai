export module hai:fn;
import no;
export import traits;

namespace hai {
export template <typename Ret, typename... Args> class fn : no::copy {
  void *m_ptr;
  Ret (*m_wrap)(void *, Args &&...);
  void (*m_del)(void *);

  void clear() {
    m_ptr = nullptr;
    m_wrap = nullptr;
    m_del = nullptr;
  }
  void del() {
    if (m_ptr && m_del)
      m_del(m_ptr);
    clear();
  }

public:
  fn(traits::is_callable_r<Ret, Args...> auto &&fn) {
    using T = traits::remove_ref_t<decltype(fn)>;
    m_ptr = new T{traits::fwd<T>(fn)};
    m_wrap = [](void *ptr, Args &&...args) -> Ret {
      return (*reinterpret_cast<T *>(ptr))(traits::fwd<Args>(args)...);
    };
    m_del = [](void *ptr) -> void { delete reinterpret_cast<T *>(ptr); };
  }
  ~fn() { del(); }

  fn(fn &&o) : m_ptr{o.m_ptr}, m_wrap{o.m_wrap}, m_del{o.m_del} { o.clear(); }
  fn &operator=(fn &&o) {
    if (&o == this)
      return *this;

    del();
    m_ptr = o.m_ptr;
    m_wrap = o.m_wrap;
    m_del = o.m_del;
    o.clear();
  }

  Ret operator()(Args &&...args) {
    return m_wrap(m_ptr, traits::fwd<Args>(args)...);
  };
};
} // namespace hai

