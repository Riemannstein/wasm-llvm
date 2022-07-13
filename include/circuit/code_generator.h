#pragma once
#include "circuit/utils.h"
namespace circuit
{
class Variable;
class FuncPrototype;
class Func;
class ExpressionVisitor
{
public:
  // virtual void visit(Module* module) = 0;
  virtual llvm::Function* visit(FuncPrototype* func_prototype) = 0;
  virtual llvm::Function* visit(Func* func) = 0;
  virtual llvm::Argument* visit(Variable* variable) = 0;
  virtual ~ExpressionVisitor() = default;
};

class CodeGenerator : public ExpressionVisitor
{
  shared_ptr<LLVMContext> llvm_context_;
  shared_ptr<llvm::Module> llvm_module_;
  shared_ptr<llvm::IRBuilder<>> ir_builder_;
  // TargetMachine* target_machine_;
public:
  CodeGenerator(
    shared_ptr<LLVMContext> llvm_context, 
    shared_ptr<llvm::Module> llvm_module, 
    shared_ptr<llvm::IRBuilder<>> ir_builder
  ) :
    llvm_context_(llvm_context),
    llvm_module_(llvm_module),
    ir_builder_(ir_builder)
  {

  }
  CodeGenerator(){
    llvm_context_= make_shared<LLVMContext>();
    llvm_module_ = make_shared<llvm::Module>("my-module", *llvm_context_);
    ir_builder_ = make_shared<llvm::IRBuilder<>>(*llvm_context_);
  }
  // void visit(Module* module) override{};
  llvm::Function* visit(FuncPrototype* func_prototype) override;
  llvm::Function* visit(Func* func) override;
  llvm::Argument* visit(Variable* variable) override;
  void print(){
    llvm_module_->print(llvm::errs(), nullptr);
  }
};


}