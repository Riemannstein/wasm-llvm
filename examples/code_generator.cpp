#include "circuit/parser.h"
#include "circuit/lexer.h"
#include "circuit/code_generator.h"
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

  // Create Lexer
  istringstream iss(source);
  auto lexer = make_unique<Lexer>(iss);

  // llvm context, module, ir_builder
  auto llvm_context= make_shared<LLVMContext>();
  auto llvm_module = make_shared<llvm::Module>("my-module", *llvm_context);
  auto ir_builder = make_shared<llvm::IRBuilder<>>(*llvm_context);

  // Create CodeGenerator
  auto code_generator = make_shared<CodeGenerator>(llvm_context, llvm_module, ir_builder);

  // Create parser
  auto parser = make_unique<Parser>(move(lexer), code_generator);
  parser->run();
  code_generator->print();
  return 0;
}