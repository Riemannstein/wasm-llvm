macro(add_example NAME MODULE SRC)
  if("${MODULE}" STREQUAL "")
    add_executable(${NAME} ${SRC})
  else()
    add_executable(${NAME} ${MODULE}/${SRC})
  endif()
  if(NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Release"))
    target_compile_options(${NAME} PRIVATE "-g")
  endif()
  target_link_libraries(${NAME} PRIVATE ${PROJECT_NAME})
  set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${MODULE})
endmacro()

# lexer
add_example("lexer" "" "lexer.cpp")

# parser
add_example("parser" "" "parser.cpp")

# keyword
add_example("keyword" "" "keyword.cpp")

# utils
add_example("utils" "" "utils.cpp")


# code generator
add_example("code-generator" "" "code_generator.cpp")

# circular_example
add_example("circular-example" "" "circular_example.cpp")



# # backtest
# add_example("market-data-replayer-postgres" "backtest" "run_market_data_replayer_postgres.cpp")

# # basiclib
# add_example("utils" "basiclib" "utils.cpp")
# add_example("object-pool" "basiclib" "object_pool.cpp")
# add_example("http-client" "basiclib" "http_client.cpp")

# # mds
# add_example("deribit-mds" "mds/deribit" "run_deribit_mds.cpp")
# add_example("create-channels-mds-deribit" "mds/deribit" "create_channels.cpp")
# add_example("market-data-recorder" "mds" "run_market_data_recorder.cpp")

# # oms
# add_example("deribit-oms" "oms/deribit" "run_oms.cpp")

# #core
# add_example("strategy-env" "core" "run_strategy_env.cpp")
# add_example("instrument-manager" "core" "instrument_manager.cpp")

# # strategy
# add_example("trivial-market-making" "strategy" "run_trivial_market_making.cpp")
# add_example("option-market-making" "strategy" "run_option_market_making.cpp")
# add_example("backtest-trivial-market-making" "strategy" "run_backtest_trivial_market_making.cpp")