#include "HttpHeaders.hpp"
#include <iostream>

#define CONTENT_LENGTH 5

int main(void) {
  HttpHeaders hdrs;
  std::cout << hdrs;
  hdrs.setContentLength(CONTENT_LENGTH);
  std::cout << hdrs;
}
