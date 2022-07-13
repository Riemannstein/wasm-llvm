#pragma once
#include "circuit/utils.h"

namespace circuit
{

class Keyword{
public:
  enum class Enum {
    // others
    kFunc,
    kModule,
    kParam,
    kResult,
    kExport,
    kImport,
    kMemory,
    kType,
    kCall,

    // instruction
    kLocalGet,
    kI32Sub,

    // numeric type
    kI32
  };

  Keyword(string str) : str_(str), value_(uom_str_enum_.at(str)){
  }

  static bool is_keyword(string text){
    return uom_str_enum_.find(text)!=uom_str_enum_.end();
  }

  bool is_instruction(){
    return instructions_enum_key_.find(value_)!=instructions_enum_key_.end();
  }

  const string& str() const {return str_;}

  Enum value() const {return value_;}



private:

  string str_;
  Enum value_;

  static const inline unordered_map<string, Enum> others_{
    {"func", Enum::kFunc}, {"module", Enum::kModule}, {"module", Enum::kModule},
    {"param", Enum::kParam}, {"result", Enum::kResult}, {"export", Enum::kExport},
    {"import", Enum::kImport}, {"memory", Enum::kMemory}, {"type", Enum::kType},
    {"call", Enum::kCall}, {"i32", Enum::kI32}
  };

  static const inline unordered_map<string, Enum> instructions_string_key_{
    {"local.get", Enum::kLocalGet}, {"i32.sub", Enum::kI32Sub}
  };

  static const inline unordered_map<Enum, string> instructions_enum_key_ = 
    [](){
      unordered_map<Enum, string> instructions_enum_key;
      for(const auto& [k,v] : instructions_string_key_){
        instructions_enum_key.insert({v, k});
      }
      return instructions_enum_key;
    }();

  static const inline unordered_map<string, Enum> uom_str_enum_= 
    [](){
      unordered_map<string, Enum> uom_str_enum;
      for(const auto& [k, v] : instructions_string_key_){
        uom_str_enum.insert({k,v});
      }
      for(const auto& [k, v] : others_){
        uom_str_enum.insert({k,v});
      }

      return uom_str_enum;
    }();

  static const inline unordered_map<Enum, string> uom_enum_str_ = 
    [](){
      unordered_map<Enum, string> uom_enum_str;
      for(const auto& [k,v] : uom_str_enum_){
        uom_enum_str.insert({v, k});
      }
      return uom_enum_str;
    }();

};



  // // TODO: complete the list of keywords
  // static const unordered_set<string> keywords{
  //   "func", "module", "param", "result", "export", "import", "memory", "type",
  //   "call", 
  //   "local.get", "i32", "i32.sub"
  // };


}
