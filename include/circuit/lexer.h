#pragma once
#include "circuit/utils.h"
#include "circuit/keyword.h"
namespace circuit
{


class KeywordFilter{};

enum TokenType {
  kEOF = -1,
  kKeyword = -2,
  kReserved = -3,
  kSignedInteger = -4,
  kUninterpretedInteger = -5,
  kFloatingNumber = -6, // TODO
  kIdentifier = -7,
  kLeftParenthesis = -8,
  kRightParenthesis = -9,
  kString = -10 // TODO
};

class Token
{
public:
  Token(TokenType type, string text) : 
    type_(type), text_(text)
  {}

  Token() : type_(TokenType::kIdentifier), text_(""){};
  
  TokenType type() const {return type_;}

  void set_type(TokenType type) {type_ = type;}

  void set_text(string text) {
    text_ = text;
  } 

  const string& text() const {
    return text_;
  }

private:
  TokenType type_;
  string text_;

};

class Lexer
{
public:
  Lexer(istream& ifs): ifs_(ifs)
  {

  }

  Token next_token() {
    Token token;
    string text{""};
    // exhaust space before next token
    while(is_white_space(last_char_)){
      if(ifs_.peek()!=EOF){
        last_char_ = ifs_.get();
      }
      else{
        token.set_type(TokenType::kEOF);
        token.set_text("");
        spdlog::debug(format("{}:\n{}\n", __PRETTY_FUNCTION__, token.text()));
        return token;
      }
    }
    
    // check for parenthese
    switch(last_char_){ // non-space character
      case('('):{
        token.set_type(TokenType::kLeftParenthesis);
        token.set_text("(");
        set_next_char();
        spdlog::debug(format("{}:\n{}\n", __PRETTY_FUNCTION__, token.text()));
        return token;
      }
      case(')'):{
        token.set_type(TokenType::kRightParenthesis);
        token.set_text(")");
        set_next_char();
        spdlog::debug(format("{}:\n{}\n", __PRETTY_FUNCTION__, token.text()));
        return token;
      }
    }

    text.push_back(last_char_);

    // get the whole token
    bool is_whole_token_fetched{false};
    while(!is_whole_token_fetched){
      if(ifs_.peek()!=EOF){
        set_next_char();
        if(last_char_ == ' '){
          is_whole_token_fetched = true;
        }
        else if(last_char_ == '(' or last_char_ == ')'){
          is_whole_token_fetched = true;
        }
        else{
          text.push_back(last_char_);
        }
      }
      else{
        is_whole_token_fetched = true;
      }
    }

    token.set_text(text);

    // check for keywords
    if(is_keyword(text)){
      token.set_type(TokenType::kKeyword);
    }
    else if(is_reserved(text)){
      token.set_type(TokenType::kReserved);
    }
    else if(is_uninterpreted_integer(text)){
      token.set_type(TokenType::kUninterpretedInteger);
    }
    else if (is_signed_integer(text)){
      token.set_type(TokenType::kSignedInteger);
    }
    else if(is_identifier(text)){
      token.set_type(TokenType::kIdentifier);
    }
    else{
      throw fmt::format("NotATokenError: {}", text);
    }
    spdlog::debug(format("{}:\n{}\n", __PRETTY_FUNCTION__, token.text()));
    return token;
       
  }

  static bool is_id_char(char c) {
    static const unordered_set<char> idchar_set = {
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      '!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '/', ':', '<', '=', '>', '?', '@', '\\', '^', '_', '`', '|', '~'  
    };
    return (idchar_set.find(c) != idchar_set.end());
  }

  static bool is_keyword(string text){
    return Keyword::is_keyword(text);
  }

  static bool is_reserved(string text){
    static unordered_set<string> reserved_words = {

    };
    return (reserved_words.find(text) != reserved_words.end());
  }

  static bool is_uninterpreted_integer(string text){
    // text is non-empty
    bool value{false};
    if((text.at(0) != '+') and (text.at(0)!='-')){
      try{
        std::stoull(text);
        value = true;
      }
      catch(const std::exception& e){
      }
    }
    return value;

  }

  static bool is_signed_integer(string text){
    bool value{false};
    if((text[0] == '+') or (text[0] == '-')){
      if(text.length()>1){
        if(is_uninterpreted_integer(text.substr(1))){
          value = true;
        }
      }
    }
    return value;
  }

  static bool is_identifier(string text){
    bool value{false};
    if((text[0] == '$') and (text.length()>1)){
      bool is_non_id_char_encountered{false};
      for(long long i =1;i<text.length();++i){
        if(!is_id_char(text.at(i))){
          is_non_id_char_encountered = true;
          break;
        }
      }
      if(!is_non_id_char_encountered){
        value = true;
      }
    }
    return value;
  }

  static bool is_white_space(char c){
    static const unordered_set<char> white_space_characters{' ', '\n', 9, 13 };
    return white_space_characters.find(c) != white_space_characters.end();
  }

private:
  char last_char_ = ' ';
  istream& ifs_;

  void set_next_char(){
    if(ifs_.peek()==EOF){
      last_char_ = ' ';
    }
    else{
      last_char_ = ifs_.get();
    }
  }

};

};