#include "Logging.hpp"
#include "Networking.hpp"

int main(void) {
  logging::log("great", logging::Debug);
  logging::log("ok", logging::Info);
  logging::log("bad", logging::Warning);
  logging::log("terrible", logging::Error);

  networking::start();
}
