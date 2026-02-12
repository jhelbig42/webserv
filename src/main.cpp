#include "Logging.hpp"
#include <iostream>

int main(void) {
  logging::logString(logging::Debug, "great");
  logging::logString(logging::Info, "ok");
  logging::logString(logging::Warning, "bad");
  logging::logString(logging::Error, "terrible");

  std::cout << '\n';

  logging::log(logging::Debug, 1);
  logging::log(logging::Info, 2);
  logging::log(logging::Warning, 3);
  logging::log(logging::Error, 4);

  std::cout << '\n';

  logging::log2(logging::Debug, "first number: ", 1);
  logging::log2(logging::Info, "second number: ", 2);
  logging::log2(logging::Warning, "third number: ", 3);
  logging::log2(logging::Error, "fourth number: ", 4);

  std::cout << '\n';

  logging::log3(logging::Debug, "first number: ", 1, " is great");
  logging::log3(logging::Info, "second number: ", 2, " is ok");
  logging::log3(logging::Warning, "third number: ", 3, " is bad");
  logging::log3(logging::Error, "fourth number: ", 4, " is terrible");
}
