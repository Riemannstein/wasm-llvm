#include "circuit/parser.h"
#include "circuit/lexer.h"
#include <memory>
#include <sstream>

int main(){
  using namespace std;
  using namespace circuit;
  configure_logger(1);
  string source = "(module "
      "(func $how_old (param $year_now i32) (param $year_born i32) (result i32) "
          "local.get $year_now "
          "local.get $year_born "
          "i32.sub)\n"
  ")";
  istringstream iss(source);
  auto lexer = make_unique<Lexer>(iss);
  auto parser = make_unique<Parser>(move(lexer));
  parser->run();
  return 0;
}