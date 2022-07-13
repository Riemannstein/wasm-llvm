#define BOOST_TEST_MODULE test-lexer

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include <string>
#include <string_view>
#include <fstream>
#include <istream>
#include <sstream>
#include <iostream>
#include "circuit/lexer.h"

using std::string;
using std::string_view;
using std::ifstream;
using std::istringstream;
using namespace circuit;

BOOST_AUTO_TEST_SUITE(test_request_type_cast)

BOOST_AUTO_TEST_CASE(test_left_parenthesis){
  string source = "(";
  istringstream iss(source);
  auto lexer = Lexer(iss);
  auto token = lexer.next_token();
  BOOST_ASSERT(token.type() == TokenType::kLeftParenthesis);
  BOOST_ASSERT(token.text() == "(");
}

BOOST_AUTO_TEST_CASE(test_right_parenthesis){
  string source = ")";
  istringstream iss(source);
  auto lexer = Lexer(iss);
  auto token = lexer.next_token();
  BOOST_ASSERT(token.type() == TokenType::kRightParenthesis);
  BOOST_ASSERT(token.text() == ")");
}

BOOST_AUTO_TEST_CASE(test_eof){
  string source = "";
  istringstream iss(source);
  auto lexer = Lexer(iss);
  auto token = lexer.next_token();
  BOOST_ASSERT(token.type() == TokenType::kEOF);
}

BOOST_AUTO_TEST_CASE(test_identifier){
  string source = "$a";
  istringstream iss(source);
  auto lexer = Lexer(iss);
  auto token = lexer.next_token();
  BOOST_ASSERT(token.type() == TokenType::kIdentifier);
  BOOST_ASSERT(token.text() == source);
}

BOOST_AUTO_TEST_CASE(test_uninterpreted_integer){
  string source = "0";
  istringstream iss(source);
  auto lexer = Lexer(iss);
  auto token = lexer.next_token();
  BOOST_ASSERT(token.type() == TokenType::kUninterpretedInteger);
  BOOST_ASSERT(token.text() == source);
}

BOOST_AUTO_TEST_CASE(test_signed_integer_1){
  string source = "+1";
  istringstream iss(source);
  auto lexer = Lexer(iss);
  auto token = lexer.next_token();
  BOOST_ASSERT(token.type() == TokenType::kSignedInteger);
  BOOST_ASSERT(token.text() == source);
}

BOOST_AUTO_TEST_CASE(test_signed_integer_2){
  string source = "-1";
  istringstream iss(source);
  auto lexer = Lexer(iss);
  auto token = lexer.next_token();
  BOOST_ASSERT(token.type() == TokenType::kSignedInteger);
  BOOST_ASSERT(token.text() == source);
}

BOOST_AUTO_TEST_CASE(test_keyword){
  string source = "module";
  istringstream iss(source);
  auto lexer = Lexer(iss);
  auto token = lexer.next_token();
  BOOST_ASSERT(token.type() == TokenType::kKeyword);
  BOOST_ASSERT(token.text() == source);
}

BOOST_AUTO_TEST_CASE(test_1){
  string source = "(module "
    "(func $how_old (param $year_now i32) (param $year_born i32) (result i32) "
        "local.get $year_now "
        "local.get $year_born "
        "i32.sub)\n"
")";
  istringstream iss(source);
  auto lexer = Lexer(iss);
  Token token;
  while(token.type()!=TokenType::kEOF){
    token = lexer.next_token();
    std::cout << fmt::format("token type: {}; text: {}\n", token.type(), token.text()); 
  }

  // BOOST_ASSERT(token.type() == TokenType::kSignedInteger);
  // BOOST_ASSERT(token.text() == source);
}

BOOST_AUTO_TEST_SUITE_END()