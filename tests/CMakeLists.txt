set(Boost_USE_STATIC_LIBS OFF)
# find_package(Boost 1.77 REQUIRED COMPONENTS unit_test_framework)
macro(add_test NAME MODULE SRC)
  if("${MODULE}" STREQUAL "")
    add_executable(${NAME} ${SRC})
  else()
    add_executable(${NAME} ${MODULE}/${SRC})
  endif()
  target_compile_options(${NAME} PRIVATE "--std=c++20" "-g")
  # target_compile_definitions(${NAME} PRIVATE BOOST_TEST_DYN_LINK)
  # target_link_libraries(${NAME} PRIVATE Boost::unit_test_framework)
  target_link_libraries(${NAME} PRIVATE ${PROJECT_NAME})
  set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/tests/${MODULE})

endmacro()

## lexer
add_test("test-lexer" "" "test_lexer.cpp")

## Parser
add_test("test-parser" "" "test_parser.cpp")

# ## oms
# add_test_tg("test-deribit-oms-request" "oms/deribit" "test_request.cpp")

# ## quant
# add_test_tg("test-vol-estimator" "quant" "test_vol_estimator.cpp")
# add_test_tg("test-instrument-wrapper" "quant" "test_instrument_wrapper.cpp")