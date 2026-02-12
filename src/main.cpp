#include "Logging.hpp"
#include <iostream>

int main(void) {
  logging::log(logging::Debug, "great");
  logging::log(logging::Info, "ok");
  logging::log(logging::Warning, "bad");
  logging::log(logging::Error, "terrible");

  std::cout << '\n';

  logging::log(logging::Debug, 1);
  logging::log(logging::Info, 2);
  logging::log(logging::Warning, 3);
  logging::log(logging::Error, 4);
}
