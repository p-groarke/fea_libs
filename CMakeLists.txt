cmake_minimum_required (VERSION 3.20)
project(fea_libs VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GoogleTest)

# User options
option(FEA_TESTS "Build and run tests." On)
option(FEA_BENCHMARKS "Build and run bencharks, requires tests." Off)
option(FEA_CPP17 "Use C++17 (default is C++20). CMake's detection doesn't work well." Off)
option(FEA_CPP14 "Use C++14 (default is C++17). CMake's detection doesn't work well." Off)

# Conan search paths.
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

# Output binary to predictable location (fixes cyclic dependency issues).
set(BINARY_OUT_DIR ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# Organize unrelated targets to clean IDE hierarchy.
set(DEPENDENCY_FOLDER "Dependencies")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ${DEPENDENCY_FOLDER})

# Compile Options
function(set_compile_options REQUIRED_ARG EXPOSURE)
	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		target_compile_options(${REQUIRED_ARG} ${EXPOSURE} -Wall -Wextra -Wpedantic -Werror
		)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_compile_options(${REQUIRED_ARG} ${EXPOSURE} -Wall -Wextra -Wpedantic -Werror
		)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
		# using Intel C++
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		target_compile_definitions(${REQUIRED_ARG} ${EXPOSURE} NOMINMAX UNICODE _UNICODE
			_SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING)
		target_compile_options(${REQUIRED_ARG} ${EXPOSURE} /Zc:__cplusplus /Zc:alignedNew
			/permissive- /W4 /WX /utf-8
			# /FAs
		)
		# set_target_properties(${REQUIRED_ARG} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif()
endfunction()

# Pulls conan dependencies according to platform settings.
function(pull_conan)
	message("Pulling conan dependencies.\n")

	# message("sizeof_void : ${CMAKE_SIZEOF_VOID_P }\n")
	# message("host platform : ${CMAKE_HOST_SYSTEM_PROCESSOR}")
	# message("target platform : ${CMAKE_GENERATOR_PLATFORM}\n")

	# Cross-compiling to 32bits on Windows.
	if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES AMD64 AND CMAKE_GENERATOR_PLATFORM MATCHES Win32)
		message("Cross-compiling to 32bits.\n")
		set(CONAN_ARCH "-s arch=x86")
		separate_arguments(CONAN_ARCH UNIX_COMMAND "${CONAN_ARCH}")
	else()
		set(CONAN_ARCH)
	endif()

	if (WIN32)
		set(CONAN_TOOLSET "-s compiler.toolset=${CMAKE_VS_PLATFORM_TOOLSET}")
		separate_arguments(CONAN_TOOLSET UNIX_COMMAND "${CONAN_TOOLSET}")
	else()
		set(CONAN_TOOLSET)
	endif()

	execute_process(COMMAND conan install ${CMAKE_CURRENT_SOURCE_DIR} --build missing -s
		build_type=Debug ${CONAN_ARCH} ${CONAN_TOOLSET}
		COMMAND_ECHO STDOUT
	)
	execute_process(COMMAND conan install ${CMAKE_CURRENT_SOURCE_DIR} --build missing -s
		build_type=Release ${CONAN_ARCH} ${CONAN_TOOLSET}
		COMMAND_ECHO STDOUT
	)
endfunction()

# clang-format
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.clang-format ${CMAKE_CURRENT_BINARY_DIR}/.clang-format COPYONLY)

# C++ version.
if ("cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES AND NOT FEA_CPP17 AND NOT FEA_CPP14)
	set(CMAKE_CXX_STANDARD 20)
	message("\nEnabling C++14, C++17 and C++20 libraries.\n")
elseif ("cxx_std_17" IN_LIST CMAKE_CXX_COMPILE_FEATURES AND NOT FEA_CPP14)
	set(CMAKE_CXX_STANDARD 17)
	message("\nEnabling C++14 and C++17 libraries.\n")
else()
	set(CMAKE_CXX_STANDARD 14)
	message("\nEnabling C++14 libraries.\n")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# External dependencies.
pull_conan()
find_package(TBB CONFIG REQUIRED)
find_package(date CONFIG REQUIRED)


# Main Project
set(INCLUDE_NAME fea)
file(GLOB_RECURSE HEADER_FILES_CPP14 "include_cpp14/${INCLUDE_NAME}/*.hpp")
file(GLOB_RECURSE HEADER_FILES_CPP17 "include_cpp17/${INCLUDE_NAME}/*.hpp")
file(GLOB_RECURSE HEADER_FILES_CPP20 "include_cpp20/${INCLUDE_NAME}/*.hpp")
add_library(${PROJECT_NAME} INTERFACE)
set_compile_options(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE TBB::TBB date::date)

# To see files in IDE
# target_sources(${PROJECT_NAME} INTERFACE
# 	# $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
# 	"$<BUILD_INTERFACE:${HEADER_FILES_CPP14}>"
# 	"$<BUILD_INTERFACE:$<$<COMPILE_FEATURES:cxx_std_17>:${HEADER_FILES_CPP17}>>"
# )

# Interface
target_include_directories(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include_cpp14>
	$<BUILD_INTERFACE:$<$<COMPILE_FEATURES:cxx_std_17>:${CMAKE_CURRENT_SOURCE_DIR}/include_cpp17>>
	$<BUILD_INTERFACE:$<$<COMPILE_FEATURES:cxx_std_20>:${CMAKE_CURRENT_SOURCE_DIR}/include_cpp20>>
)

# source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include_cpp14 PREFIX "Header Files" FILES ${HEADER_FILES_CPP14})
# source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include_cpp17 PREFIX "Header Files" FILES ${HEADER_FILES_CPP17})
# make_pretty_source_folders("include_cpp14/fea" "Header Files" "${HEADER_FILES_CPP14}")
# make_pretty_source_folders("include_cpp17/fea" "Header Files" "${HEADER_FILES_CPP17}")

# Install Package Configuration
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}_targets)

install(EXPORT ${PROJECT_NAME}_targets
	NAMESPACE ${PROJECT_NAME}::
	FILE ${PROJECT_NAME}-config.cmake
	DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include_cpp14/${INCLUDE_NAME}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
if (${CMAKE_CXX_STANDARD} GREATER_EQUAL 17)
	install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include_cpp17/${INCLUDE_NAME}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()
if (${CMAKE_CXX_STANDARD} GREATER_EQUAL 20)
	install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include_cpp20/${INCLUDE_NAME}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()

# Tests
if (${FEA_TESTS})
	# enable_testing()

	# Tests external dependencies.
	find_package(GTest CONFIG REQUIRED)

	# Test Project
	set(TEST_NAME ${PROJECT_NAME}_tests)
	set(TEST_NAME_NOTHROW ${PROJECT_NAME}_nothrow_tests)

	file(GLOB_RECURSE TEST_HEADERS_CPP14 "include_cpp14/*.hpp")
	file(GLOB_RECURSE TEST_HEADERS_CPP17 "include_cpp17/*.hpp")
	file(GLOB_RECURSE TEST_HEADERS_CPP20 "include_cpp20/*.hpp")

	file(GLOB_RECURSE TEST_SOURCES_CPP14 "tests_cpp14/*.cpp" "tests_cpp14/*.hpp" "tests_cpp14/*.tpp")
	file(GLOB_RECURSE TEST_SOURCES_CPP17 "tests_cpp17/*.cpp" "tests_cpp17/*.hpp" "tests_cpp17/*.tpp")
	file(GLOB_RECURSE TEST_SOURCES_CPP20 "tests_cpp20/*.cpp" "tests_cpp20/*.hpp" "tests_cpp20/*.tpp")

	add_executable(${TEST_NAME} ${TEST_SOURCES_CPP14} ${TEST_HEADERS_CPP14}
		"$<$<COMPILE_FEATURES:cxx_std_17>:${TEST_SOURCES_CPP17}>"
		"$<$<COMPILE_FEATURES:cxx_std_17>:${TEST_HEADERS_CPP17}>"
		"$<$<COMPILE_FEATURES:cxx_std_20>:${TEST_SOURCES_CPP20}>"
		"$<$<COMPILE_FEATURES:cxx_std_20>:${TEST_HEADERS_CPP20}>"
	)
	add_executable(${TEST_NAME_NOTHROW} ${TEST_SOURCES_CPP14} ${TEST_HEADERS_CPP14}
		"$<$<COMPILE_FEATURES:cxx_std_17>:${TEST_SOURCES_CPP17}>"
		"$<$<COMPILE_FEATURES:cxx_std_17>:${TEST_HEADERS_CPP17}>"
		"$<$<COMPILE_FEATURES:cxx_std_20>:${TEST_SOURCES_CPP20}>"
		"$<$<COMPILE_FEATURES:cxx_std_20>:${TEST_HEADERS_CPP20}>"
	)

	set_compile_options(${TEST_NAME} PRIVATE)
	set_compile_options(${TEST_NAME_NOTHROW} PRIVATE)

	# Set NOTHROW and other global defines that could impact tests.
	target_compile_definitions(${TEST_NAME_NOTHROW} PRIVATE FEA_NOTHROW -DFEA_SERIALIZE_SIZE_T=uint16_t -DFEA_CODEPAGE_CONVERSIONS)

	# For the extreme serialization test.
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		target_compile_options(${TEST_NAME} PRIVATE /bigobj)
		target_compile_options(${TEST_NAME_NOTHROW} PRIVATE /bigobj)
	endif()

	# g++ 8 needs to link with seperate filesystem library.
	target_link_libraries(${TEST_NAME} PUBLIC ${PROJECT_NAME} GTest::GTest TBB::TBB
		$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},9.0>>:stdc++fs>
	)
	target_link_libraries(${TEST_NAME_NOTHROW} PUBLIC ${PROJECT_NAME} GTest::GTest TBB::TBB
		$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},9.0>>:stdc++fs>
	)


	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/tests_cpp14 PREFIX "Source Files" FILES ${TEST_SOURCES_CPP14})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include_cpp14 PREFIX "Header Files" FILES ${TEST_HEADERS_CPP14})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/tests_cpp17 PREFIX "Source Files" FILES ${TEST_SOURCES_CPP17})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include_cpp17 PREFIX "Header Files" FILES ${TEST_HEADERS_CPP17})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/tests_cpp20 PREFIX "Source Files" FILES ${TEST_SOURCES_CPP20})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include_cpp20 PREFIX "Header Files" FILES ${TEST_HEADERS_CPP20})

	if (${FEA_BENCHMARKS})
		target_compile_definitions(${TEST_NAME} PRIVATE FEA_BENCHMARKS)
		target_compile_definitions(${TEST_NAME_NOTHROW} PRIVATE FEA_BENCHMARKS)
	endif()

	# gtest_discover_tests(${TEST_NAME})
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TEST_NAME})

	# Copy test data on build.
	# set(CPP14_DATA_IN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests_cpp14/data)
	# set(CPP14_DATA_OUT_DIR ${BINARY_OUT_DIR}/tests_data)
	# add_custom_command(TARGET ${TEST_NAME} POST_BUILD
	# 	COMMAND ${CMAKE_COMMAND} -E make_directory ${CPP14_DATA_OUT_DIR}
	# 	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CPP14_DATA_IN_DIR} ${CPP14_DATA_OUT_DIR}
	# )
	set(CPP17_DATA_IN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests_cpp17/data)
	set(CPP17_DATA_OUT_DIR ${BINARY_OUT_DIR}/tests_data)
	add_custom_command(TARGET ${TEST_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CPP17_DATA_OUT_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CPP17_DATA_IN_DIR} ${CPP17_DATA_OUT_DIR}
	)
endif()
