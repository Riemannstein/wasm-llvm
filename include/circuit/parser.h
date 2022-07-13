#pragma once
#include "circuit/lexer.h"
#include "circuit/ast.h"
#include "circuit/utils.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "spdlog/spdlog.h"
#include "circuit/keyword.h"

namespace circuit
{
using llvm::LLVMContext;
using llvm::IRBuilder;
class Parser
{
  unique_ptr<Lexer> lexer_;
  shared_ptr<CodeGenerator> code_generator_;
  Token current_token_;
public:
  Parser(unique_ptr<Lexer> lexer, shared_ptr<CodeGenerator> code_generator = nullptr) : 
    lexer_(move(lexer)), code_generator_(code_generator)
  {}

  void run() {
    current_token_ = lexer_->next_token();
    main_loop();
  }

private:

  void main_loop() {
    while(true){
      switch(current_token_.type()){
        case TokenType::kEOF: {
          spdlog::debug("{} - All tokens parsed.", __PRETTY_FUNCTION__);
          return;
        }
        case TokenType::kLeftParenthesis: {
          handle_expression();
          break;
        }
        default:
          throw fmt::format("UnhandledTokenError");
      }
    }
  }

  void handel_module(){

  }

  void init_llvm_basics(){

  }

  void handle_expression(){
    current_token_  = lexer_->next_token();
    switch(current_token_.type()){
      case TokenType::kKeyword: {
        auto keyword = Keyword(current_token_.text());
        switch(keyword.value()){
          case Keyword::Enum::kModule:{
            handle_module();
            break;
          }
          case Keyword::Enum::kParam: {
            handle_param();
            break;
          }
          case Keyword::Enum::kFunc: {
            current_token_ = lexer_->next_token();
            handle_func();
            break;
          }
        }

      }
      case TokenType::kRightParenthesis: {
        current_token_ = lexer_->next_token();
        break;
      }
    }
  }

  void handle_module(){
    parse_module();
    spdlog::debug("Parsed a module.");

    // if(parse_module()){
    //   spdlog::debug("Parsed a module.");
    // }
    // else{
    //   // Skip token for error recovery
    //   current_token_ = lexer_->next_token();
    // }
  }

  unique_ptr<Expression> parse_module(){
    current_token_ = lexer_->next_token();
    string module_identifier{""};
    if(current_token_.type() == TokenType::kIdentifier){
      module_identifier = std::move(current_token_.text());
      current_token_ = lexer_->next_token();
    }
    bool is_module_parsed{false};
    while(!is_module_parsed){
      switch(current_token_.type()){
        case(TokenType::kLeftParenthesis):{
          current_token_ = lexer_->next_token();
          break;
        }
        case(TokenType::kRightParenthesis):{
          is_module_parsed = true;
          break;
        }
        case(TokenType::kKeyword):{
          auto keyword = Keyword(current_token_.text());
          switch(keyword.value()){
            case(Keyword::Enum::kFunc):{
              current_token_ = lexer_->next_token();
              handle_func();
              break;
            }
            default:
              throw format("{}:\nUnhandledModuleFied: {}", __PRETTY_FUNCTION__, current_token_.text());
          }
          break;
        }
        default:
          throw fmt::format("{}:\nUnhandledTokenError: {}", __PRETTY_FUNCTION__, current_token_.text());
      }
    }
    current_token_ = lexer_->next_token();
    return nullptr;
  }

  unique_ptr<Param> handle_param(){
    auto param = parse_param();
    return param;
  }

  unique_ptr<Param> parse_param(){
    spdlog::debug(format("{}", __PRETTY_FUNCTION__));
    unique_ptr<Param> param{nullptr};
    string identifier{""};
    bool is_param_parsed{false};
    while(!is_param_parsed){
      switch(current_token_.type()){
        case TokenType::kIdentifier:{
          identifier = std::move(current_token_.text());
          current_token_ = lexer_->next_token();
          break;
        }
        case TokenType::kRightParenthesis:{
          is_param_parsed = true;
          break;
        }
        default:{
          auto val_type = ValType(current_token_.text());
          param = make_unique<Param>(val_type, identifier);
          current_token_ = lexer_->next_token();
        }
      }
    }
    current_token_ = lexer_->next_token();
    return param;
  }

  unique_ptr<Result> handle_result() {
    auto result = parse_result();
    return result;
  }

  unique_ptr<Result> parse_result(){
    spdlog::debug(format("{}", __PRETTY_FUNCTION__));
    auto val_type = ValType(current_token_.text());
    auto result = make_unique<Result>(val_type);
    current_token_ = lexer_->next_token();
    assert(current_token_.type() == TokenType::kRightParenthesis);
    current_token_ = lexer_->next_token();
    return result;
  }

  unique_ptr<Instruction> handle_instruction() {
    auto instruction = parse_instruction();
    if(!instruction){
      throw format("{} - InstructionNotParsedError");
    }
    spdlog::debug("{} - parsed instructions.", __PRETTY_FUNCTION__);
    return instruction;

  }

  unique_ptr<Instruction> parse_instruction(){
    spdlog::debug(format("{}", __PRETTY_FUNCTION__));
    unique_ptr<Instruction> instruction{nullptr};
    vector<unique_ptr<Expression>> operands;
    auto instruction_operator = Keyword(current_token_.text());
    assert(instruction_operator.is_instruction());
    switch(instruction_operator.value()){
      case Keyword::Enum::kLocalGet: {
        current_token_ = lexer_->next_token();
        switch(current_token_.type()){
          case TokenType::kIdentifier:{
            auto variable = make_unique<Variable>(nullptr, current_token_.text());
            operands.push_back(std::move(variable));
            instruction = make_unique<Instruction>(instruction_operator, move(operands));
            current_token_ = lexer_->next_token();
            break;
          }
          default:
            throw fmt::format("{}: Identifier expected", __PRETTY_FUNCTION__);
        }
        break;
      }
      case Keyword::Enum::kI32Sub:{
        instruction = make_unique<Instruction>(instruction_operator);
        current_token_ = lexer_->next_token();
        break;
      }
      default:
        throw fmt::format("{}: {}", __PRETTY_FUNCTION__, Exception::kInstructionOpNotHandledError);
    }
    return instruction;
  }

  void handle_func() {
    auto func = parse_func();

    code_generator_->visit(func.get());
    spdlog::debug("{} - parsed a func.", __PRETTY_FUNCTION__);

  }

  unique_ptr<Func> parse_func(){
    spdlog::debug(format("{}", __PRETTY_FUNCTION__));
    bool is_func_parsed{false};
    string identifier{""};
    vector<unique_ptr<Param>> params{};
    vector<unique_ptr<Result>> results{};
    vector<unique_ptr<Instruction>> body{};
    unique_ptr<Func> func{nullptr};
    bool is_param_done{false};
    bool is_result_done{false};
    bool is_func_done{false};
    if(current_token_.type()==TokenType::kIdentifier){
      identifier = std::move(current_token_.text());
      current_token_ = lexer_->next_token();
    }
    while(!is_func_done){
      switch(current_token_.type()){
        case TokenType::kRightParenthesis:{
          is_func_done = true;
          current_token_ = lexer_->next_token();
          break;
        }
        case TokenType::kLeftParenthesis:{
          // consume '('
          current_token_ = lexer_->next_token();
          // assert(current_token_.type() == TokenType::kKeyword);
          // auto keyword = Keyword(current_token_.text());

            
          // }
          break;
        }
        case TokenType::kKeyword:{
          Keyword keyword{current_token_.text()};
          if(keyword.is_instruction()){
            auto instruction = handle_instruction();
            body.push_back(move(instruction));
          }
          else{
            switch(keyword.value()){
              case Keyword::Enum::kParam:{
                current_token_ = lexer_->next_token();
                auto param = handle_param();
                params.push_back(move(param));
                break;
              }
              case Keyword::Enum::kResult:{
                current_token_ = lexer_->next_token();
                auto result = handle_result();
                results.push_back(move(result));
                break;
              }
                
            }
          }
          break;
        }
      }
    }

    func = make_unique<Func>(identifier, move(params), move(results), move(body));
    return func;    
  }



};

}