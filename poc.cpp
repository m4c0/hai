import hai;

int main() {
  auto p = hai::uptr<int>::make(0);
  return *p;
}
