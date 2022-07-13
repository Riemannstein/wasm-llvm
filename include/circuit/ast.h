#pragma once
#include "circuit/utils.h"
#include "circuit/code_generator.h"
namespace circuit
{
class Expression {
public:
  virtual llvm::Value* accept(ExpressionVisitor* visitor) = 0;
  virtual ~Expression() =default;
};


class ModuleField : public Expression
{
public:
  virtual ~ModuleField() = default;
};

class Module : public Expression
{
public:
  llvm::Value* accept(ExpressionVisitor* visitor) override {
    throw "NotImplementedError";
    return nullptr;
  }
private:
  string id_;
  vector<unique_ptr<ModuleField>> fields_;

};

enum class ModuleFieldType
{
 kType,
 kImport,
 kFunc,
 kTable,
 kMemory,
 kGlobal,
 kExport,
 kStart,
 kElem,
 kData
};


class ValType
{
public:
  enum class Enum{
    kI32,
    kI64,
    kF32,
    kF64,
    kV128,
    kFuncref,
    kExternref
  };

  ValType(Enum type):type_(type), str_(uom_enum_str_.at(type)){};

  ValType(const string_view& str):str_(str), type_(uom_str_enum_.at(string(str))){
    
  };

  ValType(const ValType& val_type): type_(val_type.type()), str_(uom_enum_str_.at(val_type.type())){}

  ValType() = delete;

  Enum type() const {return type_;}

  const string& str() const {return str_;}

  bool is_num_type();

  bool is_vec_type();

  bool is_ref_type();
private:
  Enum type_;
  const string str_;
  static const inline unordered_map<Enum, string> uom_enum_str_{
    {Enum::kI32, "i32"}, {Enum::kI64, "i64"}, {Enum::kF32, "f32"}, {Enum::kF64, "f64"},
    {Enum::kV128, "v128"},
    {Enum::kFuncref, "funcref"}, {Enum::kExternref, "externref"}
  };
  static const inline unordered_map<string, Enum> uom_str_enum_{
    {"i32", Enum::kI32}, {"i64", Enum::kI64}, {"f32", Enum::kF32}, {"f64", Enum::kF64},
    {"v128", Enum::kV128},
    {"funcref", Enum::kFuncref}, {"externref", Enum::kExternref}  
  };
};

class Variable : public Expression
{
  unique_ptr<ValType> val_type_;
  string identifier_;
public:
  Variable(unique_ptr<ValType> val_type = nullptr, string identifier = "") : val_type_(std::move(val_type)), identifier_(identifier){
  }

  llvm::Value* accept(ExpressionVisitor* visitor) {
    throw fmt::format("{}: {}", __PRETTY_FUNCTION__, Exception::kNotImplementedError);
    return nullptr;
  };

  const unique_ptr<ValType>& val_type() const {return val_type_;}

  void set_val_type(unique_ptr<ValType> val_type) {val_type_ = move(val_type);}

  const string& identifier() const {return identifier_;}
};

llvm::Argument* CodeGenerator::visit(Variable* variable) {
  llvm::Type* llvm_type{nullptr};
  switch(variable->val_type()->type()){
    case ValType::Enum::kI32:
      llvm_type = llvm::Type::getInt32Ty(*llvm_context_);
      break;
    default:
      throw fmt::format("{}: NotImplementedError.", __PRETTY_FUNCTION__);
  }
  auto llvm_variable = new llvm::Argument(llvm_type, variable->identifier());
  return llvm_variable;
}

class Param : public Expression
{
public:
  Param(const ValType& val_type, const string_view identifier = "")
  : val_type_(val_type), identifier_(identifier)
  {}
  llvm::Value* accept(ExpressionVisitor* visitor) override {
    throw "NotImplementedError";
    return nullptr;
  }

  const ValType& val_type() const {return val_type_;}
  const string& identifier() const {return identifier_;}
  
private:
  string identifier_;
  const ValType val_type_;

  
};

class Result : public Expression
{
public:
  Result(const ValType& val_type):val_type_(val_type){}
  llvm::Value* accept(ExpressionVisitor* visitor) override {
    throw "NotImplementedError";
    return nullptr;
  }

  const ValType& val_type(){return val_type_;}
private:
  const ValType val_type_;

};


class Instruction : public Expression
{
  Keyword op_;
  vector<unique_ptr<Expression>> operands_;
public:

  Instruction(Keyword op, vector<unique_ptr<Expression>>&& operands = {}):
    op_(op), operands_(std::move(operands))
  {}

  Keyword op() const {return op_;}

  const vector<unique_ptr<Expression>>& operands() const {return operands_;}

  llvm::Value* accept(ExpressionVisitor* visitor) override {
    throw "NotImplementedError";
    return nullptr;
  }

  virtual ~Instruction() = default;
};

class NumericInstruction : public Instruction
{
  public:
  llvm::Value* accept(ExpressionVisitor* visitor) override {
    throw "NotImplementedError";
    return nullptr;
  }

};

class FuncPrototype : public Expression
{
  vector<unique_ptr<Param>> params_;
  vector<unique_ptr<Result>> results_;
  string identifier_;
public:

  FuncPrototype(
    string identifier, 
    vector<unique_ptr<Param>>&& params,  
    vector<unique_ptr<Result>>&& results
  ):
    identifier_(identifier),
    params_(move(params)),
    results_(move(results))
  {

  }

  llvm::Function* accept(ExpressionVisitor* visitor) override{return visitor->visit(this);};

  const vector<unique_ptr<Param>>& params() const {
    return params_;
  }

  const vector<unique_ptr<Result>>& results() const {
    return results_;
  }

  const string& identifier() const {
    return identifier_;
  }

};

class Func : public Expression
{
  unique_ptr<FuncPrototype> prototype_;
  vector<unique_ptr<Instruction>> body_; 
public:
  Func(
    string identifier, 
    vector<unique_ptr<Param>>&& params,  
    vector<unique_ptr<Result>>&& results,
    vector<unique_ptr<Instruction>>&& body
  ):
    prototype_(make_unique<FuncPrototype>(identifier, move(params), move(results))),
    body_(move(body))
  {

  }

  llvm::Value* accept(ExpressionVisitor* visitor) override {
    return visitor->visit(this);
  }

  FuncPrototype* prototype() const {
    return prototype_.get();
  }

  const vector<unique_ptr<Instruction>>& body(){
    return body_;
  }

};

llvm::Function* CodeGenerator::visit(Func* func){
  auto llvm_function = llvm_module_->getFunction(func->prototype()->identifier());
  auto func_prototype = func->prototype();
  if(!llvm_function){
    llvm_function = visit(func_prototype);
  }
  auto bb = llvm::BasicBlock::Create(*llvm_context_, "entry", llvm_function);
  ir_builder_->SetInsertPoint(bb);

  // Record the function arguments in the NamedValues map.
  vector<llvm::Value*> arguments;
  for(auto& arg : llvm_function->args()){
    arguments.emplace_back(&arg);
  }

  stack<llvm::Value*> wasm_stack; 

  // Generate body
  for(auto& instruction : func->body()){
    switch(instruction->op().value()){
      case Keyword::Enum::kLocalGet:{
        auto variable =static_cast<Variable*>(instruction->operands().at(0).get());

        // Get variable type from function prototype
        bool is_variable_found{false};
        for(auto& arg : arguments){
          if(variable->identifier() == arg->getName()){
            // auto first_llvm_value = visit(first_operand);
            // variable->set_val_type(make_unique<ValType>(param->val_type().type()));
            wasm_stack.push(arg);
            is_variable_found = true;
            break;
          }
        }
        if(!is_variable_found){
          throw format("{} - LocalVariableNotFound.");
        }
        break;
      }
      case Keyword::Enum::kI32Sub:{
        // TODO: add exception handling "trap" case
        auto first_llvm_value = wasm_stack.top();
        wasm_stack.pop();
        auto second_llvm_value = wasm_stack.top();
        wasm_stack.pop();
        auto result_variable = ir_builder_->CreateAdd(first_llvm_value, second_llvm_value);
        wasm_stack.push(result_variable);
        break;
      }
      default:{
        throw format("{} - InstructionNotHandled");
      }
    }
  }
  // Create return value, currently only support single return value,
  if(!wasm_stack.empty()){
    ir_builder_->CreateRet(wasm_stack.top());
    llvm::verifyFunction(*llvm_function);
  }

  return llvm_function;
}



enum class DeclarationType
{
  kModule,
  kFunc,
  kParam
};

llvm::Function* CodeGenerator::visit(FuncPrototype* func_prototype) {
  // TODO: support different parameter types
  vector<llvm::Type*> llvm_params;

  for(auto& param : func_prototype->params() ){
    switch(param->val_type().type()){
      case ValType::Enum::kI32: {
        auto param_type = llvm::Type::getInt32Ty(*llvm_context_);
        llvm_params.push_back(param_type);
        break;
      }
      default:
        throw format("{} - NotHandledParamType");
    }
  }
  // vector<llvm::Type*> llvm_params(func_prototype->params().size(), 
  //   llvm::IntegerType::getInt32Ty(*llvm_context_));

  // Currently only support one return value
  llvm::Type* ret_type{nullptr};
  switch(func_prototype->results().at(0)->val_type().type()){
    case ValType::Enum::kI32: {
      ret_type = llvm::Type::getInt32Ty(*llvm_context_);
      break;
    }
    default:
      throw format("{} - NotHandledType");
  }
  
  auto function_type = llvm::FunctionType::get(ret_type, llvm_params, false);

  auto function = llvm::Function::Create(function_type, llvm::Function::PrivateLinkage, func_prototype->identifier(), *llvm_module_);

  // Set paramter names
  int arg_count{0};
  for(auto& arg: function->args()){
    arg.setName(func_prototype->params().at(arg_count)->identifier());
    ++arg_count;
  }
  return function;
};

}

