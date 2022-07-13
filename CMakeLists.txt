cmake_minimum_required(VERSION 3.16)
project(circuit LANGUAGES C CXX)
set(accepted_cmake_build_type "Debug;Release")
if("${CMAKE_BUILD_TYPE}" IN_LIST accepted_cmake_build_type)
	message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "")
	message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
else()
	message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

# Find LLVM package
find_package(LLVM REQUIRED CONFIG)
message("LLVM_DIR: ${LLVM_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Set c++ standard to 20
SET(CMAKE_CXX_STANDARD 20)
SET(CONAN_CMAKE_CXX_STANDARD 20)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
	message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
	file(
		DOWNLOAD 
		"https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
		"${CMAKE_BINARY_DIR}/conan.cmake"
		TLS_VERIFY ON
	)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(
	CONANFILE conanfile.txt
	BASIC_SETUP
	BUILD missing
	BUILD_TYPE Release
)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_subdirectory(examples)
if(DEFINED MY_TEST)
	add_subdirectory(tests)
endif()

add_library(${PROJECT_NAME} INTERFACE)

target_compile_options(${PROJECT_NAME} INTERFACE "-std=c++20" "-fcoroutines")
# target_compile_options(${PROJECT_NAME} PRIVATE "-fPIC")

if(NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Release"))
	target_compile_options(${PROJECT_NAME} INTERFACE "-g")
endif()

target_include_directories(
	${PROJECT_NAME}
	INTERFACE
	${PROJECT_NAME}
	${PROJECT_SOURCE_DIR}/include/
	${LLVM_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(llvm_libs support core)
target_link_libraries(${PROJECT_NAME} INTERFACE ${CONAN_LIBS} ${llvm_libs})