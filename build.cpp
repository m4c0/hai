#include "build.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  auto example = unit::create<exe>("example.exe");
  example->add_ref(hai());
  example->add_unit<>("example");

  return run_main(example, argc, argv);
}
