cmake_minimum_required (VERSION 3.20)
project(fea_libs VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GoogleTest)

# User options, build related.
option(FEA_TESTS "Build and run tests." On)
option(FEA_BENCHMARKS "Build and run bencharks, requires tests." Off)
option(FEA_CPP17 "Use C++17 (default is C++20)." Off)
option(FEA_CPP14 "Use C++14 (default is C++20)." Off)
option(FEA_PULL_CONAN "Executes conan install from cmake." On)
option(FEA_CONAN2 "If pulling conan dependencies, are we running conan 2?" Off)
option(FEA_CMAKE_LOCAL "Use local fea_cmake repo. Searches for '../fea_cmake'" Off)

# Options that affect which modules and functionality are available.
option(FEA_WITH_TBB "Pull oneTBB and use it." On)

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

# Pull fea_cmake helper library.
if (${FEA_CMAKE_LOCAL})
	set(FETCHCONTENT_SOURCE_DIR_FEA_CMAKE ${CMAKE_CURRENT_SOURCE_DIR}/../fea_cmake CACHE INTERNAL "")
endif()
FetchContent_Declare(fea_cmake
	GIT_REPOSITORY https://github.com/p-groarke/fea_cmake.git
	GIT_TAG 3af8de8dee6b1e7b9a782076f90216ea4cbd10f3
)
FetchContent_MakeAvailable(fea_cmake)

# Include cmake helpers.
include(${fea_cmake_SOURCE_DIR}/CMakeLists.txt)

# Use standard fea_cmake settings.
fea_standard_settings()

# Use fea clang-format file.
fea_import_clangformat()

# Use fea gitignore file.
fea_import_gitignore()

# Output to predictable location.
fea_set_output_dir(${CMAKE_BINARY_DIR}/bin)

# External dependencies.
if (${FEA_PULL_CONAN})
	fea_bool_to_pybool(FEA_WITH_TBB _FEA_WITH_TBB_PYBOOL)

	if (NOT ${FEA_CONAN2})
		# Enable conan revisions (RREV).
		fea_execute_process(COMMAND conan config set general.revisions_enabled=True
			COMMAND_ECHO STDOUT
			ERROR_MSG "Failed to set conan 1 revisions_enabled."
		)
		fea_pull_conan(CONAN_MSVC_TOOLSET "${CMAKE_VS_PLATFORM_TOOLSET}" CONAN_ARGS "-o fea_libs:with_tbb=${_FEA_WITH_TBB_PYBOOL}")
	else()
		fea_pull_conan2(CONAN_MSVC_TOOLSET "${CMAKE_VS_PLATFORM_TOOLSET}" CONAN_ARGS "-o fea_libs:with_tbb=${_FEA_WITH_TBB_PYBOOL}")
	endif()

endif()

find_package(date CONFIG REQUIRED QUIET)


# Main Project
set(INCLUDE_NAME fea)
file(GLOB_RECURSE HEADER_FILES_CPP14 "include_cpp14/${INCLUDE_NAME}/*.hpp")
file(GLOB_RECURSE HEADER_FILES_CPP17 "include_cpp17/${INCLUDE_NAME}/*.hpp")
file(GLOB_RECURSE HEADER_FILES_CPP20 "include_cpp20/${INCLUDE_NAME}/*.hpp")
add_library(${PROJECT_NAME} INTERFACE)
fea_set_compile_options(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE date::date)

if (FEA_WITH_TBB)
	find_package(TBB CONFIG REQUIRED QUIET)
	target_link_libraries(${PROJECT_NAME} INTERFACE TBB::tbb)
else()
	# default is on.
	target_compile_definitions(${PROJECT_NAME} INTERFACE FEA_NO_TBB_DEF)
endif()

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

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "install" CACHE PATH "..." FORCE)
endif()

# Tests
if (FEA_TESTS)
	# enable_testing()

	# Tests external dependencies.
	find_package(GTest CONFIG REQUIRED)

	# Test Project
	set(TEST_NAME ${PROJECT_NAME}_tests)
	set(TEST_NAME_NOTHROW ${PROJECT_NAME}_nothrow_tests)
	set(TEST_NAME_PIPE ${PROJECT_NAME}_pipe_tests)

	file(GLOB_RECURSE TEST_HEADERS_CPP14 "include_cpp14/*.hpp")
	file(GLOB_RECURSE TEST_HEADERS_CPP17 "include_cpp17/*.hpp")
	file(GLOB_RECURSE TEST_HEADERS_CPP20 "include_cpp20/*.hpp")

	file(GLOB_RECURSE TEST_SOURCES_CPP14 "tests_cpp14/*.cpp" "tests_cpp14/*.hpp" "tests_cpp14/*.tpp")
	file(GLOB_RECURSE TEST_SOURCES_CPP17 "tests_cpp17/*.cpp" "tests_cpp17/*.hpp" "tests_cpp17/*.tpp")
	file(GLOB_RECURSE TEST_SOURCES_CPP20 "tests_cpp20/*.cpp" "tests_cpp20/*.hpp" "tests_cpp20/*.tpp")

	file(GLOB_RECURSE PIPE_SOURCES "tests_pipe/*.cpp")

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
	add_executable(${TEST_NAME_PIPE} ${PIPE_SOURCES} ${TEST_HEADERS_CPP14})

	fea_set_compile_options(${TEST_NAME} PRIVATE)
	fea_set_compile_options(${TEST_NAME_NOTHROW} PRIVATE)
	fea_set_compile_options(${TEST_NAME_PIPE} PRIVATE)

	set_target_properties(${TEST_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "--gtest_catch_exceptions=0 --gtest_filter=*")
	set_target_properties(${TEST_NAME_NOTHROW} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "--gtest_catch_exceptions=0 --gtest_filter=*")
	file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/bin/tests_data/pipe.txt" PIPE_FILEPATH)
	set_target_properties(${TEST_NAME_PIPE} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "< \"${PIPE_FILEPATH}\"")

	# Set NOTHROW and other global defines that could impact tests.
	target_compile_definitions(${TEST_NAME_NOTHROW} PRIVATE
		FEA_NO_TBB_DEF
		FEA_NOTHROW_DEF
		FEA_CODEPAGE_CONVERSIONS_DEF
		FEA_SERIALIZE_SIZE_T_DEF=uint16_t
	)

	# For the extreme serialization test.
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		target_compile_options(${TEST_NAME} PRIVATE /bigobj)
		target_compile_options(${TEST_NAME_NOTHROW} PRIVATE /bigobj)
	endif()

	# g++ 8 needs to link with seperate filesystem library.
	target_link_libraries(${TEST_NAME} PUBLIC ${PROJECT_NAME} GTest::GTest
		$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},9.0>>:stdc++fs>
	)
	target_link_libraries(${TEST_NAME_NOTHROW} PUBLIC ${PROJECT_NAME} GTest::GTest
		$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},9.0>>:stdc++fs>
	)
	target_link_libraries(${TEST_NAME_PIPE} PUBLIC ${PROJECT_NAME} GTest::GTest
		$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},9.0>>:stdc++fs>
	)

	if (FEA_WITH_TBB)
		target_link_libraries(${TEST_NAME} PUBLIC TBB::tbb)
		target_link_libraries(${TEST_NAME_NOTHROW} PUBLIC TBB::tbb)
		target_link_libraries(${TEST_NAME_PIPE} PUBLIC TBB::tbb)
	endif()

	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/tests_cpp14 PREFIX "Source Files" FILES ${TEST_SOURCES_CPP14})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include_cpp14 PREFIX "Header Files" FILES ${TEST_HEADERS_CPP14})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/tests_cpp17 PREFIX "Source Files" FILES ${TEST_SOURCES_CPP17})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include_cpp17 PREFIX "Header Files" FILES ${TEST_HEADERS_CPP17})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/tests_cpp20 PREFIX "Source Files" FILES ${TEST_SOURCES_CPP20})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include_cpp20 PREFIX "Header Files" FILES ${TEST_HEADERS_CPP20})
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/tests_pipe PREFIX "Source Files" FILES ${PIPE_SOURCES})

	if (${FEA_BENCHMARKS})
		target_compile_definitions(${TEST_NAME} PRIVATE FEA_BENCHMARKS_DEF)
		target_compile_definitions(${TEST_NAME_NOTHROW} PRIVATE FEA_BENCHMARKS_DEF)
		target_compile_definitions(${TEST_NAME_PIPE} PRIVATE FEA_BENCHMARKS_DEF)
	endif()

	# gtest_discover_tests(${TEST_NAME})
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TEST_NAME})

	# Copy test data on build.
	fea_copy_on_build(${TEST_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/tests_cpp17/data $<TARGET_FILE_DIR:${TEST_NAME}>/tests_data)
	fea_copy_on_build(${TEST_NAME_PIPE} ${CMAKE_CURRENT_SOURCE_DIR}/tests_pipe/data $<TARGET_FILE_DIR:${TEST_NAME_PIPE}>/tests_data)
endif()
