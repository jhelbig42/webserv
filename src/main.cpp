#include "Logging.hpp"

int main(void) {
  log::log("great", log::Debug);
  log::log("ok", log::Info);
  log::log("bad", log::Warning);
  log::log("terrible", log::Error);
}
