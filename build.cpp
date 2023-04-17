#include "build.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  // Tests if we can link properly. In LLVM-15, we had duplicate symbol issues
  // with virtuals and the global new/delete operators.
  auto p = unit::create<exe>("poc");
  p->add_ref(hai());
  return run_main(p, argc, argv);
}
