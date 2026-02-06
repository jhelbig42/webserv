#include "Logging.hpp"

int main(void) {
  Log::log("great", Log::DEBUG);
  Log::log("ok", Log::INFO);
  Log::log("bad", Log::WARNING);
  Log::log("terrible", Log::ERROR);
}
