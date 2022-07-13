#pragma once
#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <fstream>
#include <exception>
#include <fmt/format.h>
#include <unordered_set>
#include <unordered_map>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include "llvm/Target/TargetMachine.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DerivedTypes.h"
#include <boost/bimap.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include "circuit/exception.h"
#include <stack>

namespace circuit
{
using std::string;
using std::string_view;
using std::unique_ptr;
using std::shared_ptr;
using std::vector;
using std::make_unique;
using std::make_shared;
using std::ifstream;
using std::istream;
using std::unique_ptr;
using std::move;
using std::unordered_set;
using std::unordered_map;
using std::static_pointer_cast;
using boost::bimap;
using std::vector;
using fmt::format;
using llvm::LLVMTargetMachine;
using llvm::LLVMContext;
using std::stack;

void configure_logger(
  int logging_level,
  string log_file_name ="log.txt",
  bool is_console_sink_added = true,
  int file_size_in_mb = 100, 
  int max_files = 5,
  bool rotate_on_open = false
){

  // declare sink
  std::vector<spdlog::sink_ptr> sinks;

  // Add file sink
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
    log_file_name,
    1048576*file_size_in_mb,
    max_files,
    rotate_on_open
  );
  file_sink->set_level(static_cast<spdlog::level::level_enum>(logging_level));
  sinks.push_back(file_sink);

  // Add console sink
  if(is_console_sink_added){
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(static_cast<spdlog::level::level_enum>(logging_level));
    sinks.push_back(console_sink);
  }

  // Register logger
  std::shared_ptr<spdlog::logger> logger{nullptr};

  logger = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());
  
  logger->set_level(static_cast<spdlog::level::level_enum>(logging_level));
  logger->set_pattern("[%Y-%m-%dT%H:%M:%S.%f%z] [%n] [%^%l%$] [thread %t] %v");



  // Set flush policy
  if(static_cast<spdlog::level::level_enum>(logging_level)==spdlog::level::level_enum::debug){
    logger->flush_on(spdlog::level::debug);
  }
  else{
    spdlog::flush_every(std::chrono::seconds(5));
  }

  spdlog::set_default_logger(logger);
  // spdlog::register_logger(logger);
}
}