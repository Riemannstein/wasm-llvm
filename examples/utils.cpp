#include "circuit/utils.h"

int main(){
  using namespace circuit;
  configure_logger(1);
  spdlog::debug("hello");

  spdlog::info("hello");

  return 0;
}