#define BOOST_TEST_MODULE test-parser

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include <string>
#include <string_view>
#include <fstream>
#include <istream>
#include <sstream>
#include <iostream>
#include "circuit/lexer.h"
#include "circuit/parser.h"
#include "circuit/utils.h"

using std::string;
using std::string_view;
using std::ifstream;
using std::istringstream;
using namespace circuit;

BOOST_AUTO_TEST_SUITE(test_request_type_cast)


BOOST_AUTO_TEST_CASE(test_1){
  configure_logger(0);
  string source = "(module "
    "(func $how_old (param $year_now i32) (param $year_born i32) (result i32) "
        "local.get $year_now "
        "local.get $year_born "
        "i32.sub)\n"
")";
  istringstream iss(source);
  auto lexer = make_unique<Lexer>(iss);
  auto parser = Parser(std::move(lexer));

  parser.run();
  // Token token;
  // while(token.type()!=TokenType::kEOF){
  //   token = lexer.next_token();
  //   std::cout << fmt::format("token type: {}; text: {}\n", token.type(), token.text()); 
  // }
}

BOOST_AUTO_TEST_SUITE_END()