include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.conan.txt)

cmake_minimum_required (VERSION 3.15)
project(fea_libs VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GoogleTest)

# User options
option(FEA_TESTS "Build and run tests." On)
option(FEA_BENCHMARKS "Build and run bencharks, requires tests." Off)
option(FEA_CPP14 "Use C++14 (default is C++17). CMake's detection doesn't work well." Off)

# Conan search paths.
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

# Output binary to predictable location (fixes cyclic dependency issues).
set(BINARY_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
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

# IDE Folders
function(make_pretty_source_folders BASE_PATH PREFIX_PATH REQUIRED_ARG)
	foreach(SOURCE IN ITEMS ${REQUIRED_ARG})
		get_filename_component(SOURCE_PATH "${SOURCE}" PATH)
		# message("pretty source_path : ${SOURCE_PATH}")
		file(RELATIVE_PATH SOURCE_PATH_REL "${CMAKE_CURRENT_SOURCE_DIR}/${BASE_PATH}" "${SOURCE_PATH}")
		# message("pretty source_path_rel : ${SOURCE_PATH_REL}")
		string(REPLACE "/" "\\" GROUP_PATH "${PREFIX_PATH}/${SOURCE_PATH_REL}")
		# message("pretty group_path : ${GROUP_PATH}")
		source_group("${GROUP_PATH}" FILES "${SOURCE}")
	endforeach()
endfunction()

# Compile Options
function(set_compile_options REQUIRED_ARG EXPOSURE)
	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		target_compile_options(${REQUIRED_ARG} ${EXPOSURE} -Wall -Wextra -Wpedantic -Werror
			-Wno-gnu-zero-variadic-macro-arguments
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

# clang-format
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.clang-format ${CMAKE_CURRENT_BINARY_DIR}/.clang-format COPYONLY)

# C++ version.
if ("cxx_std_17" IN_LIST CMAKE_CXX_COMPILE_FEATURES AND NOT FEA_CPP14)
	set(CMAKE_CXX_STANDARD 17)
	message("\nEnabling C++14 and C++17 libraries.\n")
else()
	set(CMAKE_CXX_STANDARD 14)
	message("\nEnabling C++14 libraries.\n")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# External dependencies.
find_package(TBB CONFIG REQUIRED)
find_package(date CONFIG REQUIRED)


# Main Project
file(GLOB_RECURSE HEADER_FILES_CPP14 "include_cpp14/${PROJECT_NAME}/*.hpp")
file(GLOB_RECURSE HEADER_FILES_CPP17 "include_cpp17/${PROJECT_NAME}/*.hpp")
add_library(${PROJECT_NAME} INTERFACE)
set_compile_options(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE TBB::TBB date::date)

# To see files in IDE
target_sources(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	"$<BUILD_INTERFACE:${HEADER_FILES_CPP14}>"
	"$<BUILD_INTERFACE:$<$<COMPILE_FEATURES:cxx_std_17>:${HEADER_FILES_CPP17}>>"
)

# Interface
target_include_directories(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include_cpp14>
	$<BUILD_INTERFACE:$<$<COMPILE_FEATURES:cxx_std_17>:${CMAKE_CURRENT_SOURCE_DIR}/include_cpp17>>
)

make_pretty_source_folders("include_cpp14/fea" "Header Files" "${HEADER_FILES_CPP14}")
make_pretty_source_folders("include_cpp17/fea" "Header Files" "${HEADER_FILES_CPP17}")

# Install Package Configuration
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}_targets)

install(EXPORT ${PROJECT_NAME}_targets
	NAMESPACE ${PROJECT_NAME}::
	FILE ${PROJECT_NAME}-config.cmake
	DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include_cpp14/${PROJECT_NAME}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
if (${CMAKE_CXX_STANDARD} GREATER_EQUAL 17)
	install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include_cpp17/${PROJECT_NAME}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()

# Tests
if (${FEA_TESTS})
	enable_testing()

	# Tests external dependencies.
	find_package(GTest CONFIG REQUIRED)

	# Test Project
	set(TEST_NAME ${PROJECT_NAME}_tests)
	file(GLOB_RECURSE TEST_SOURCES_CPP14 "tests_cpp14/*.cpp" "tests_cpp14/*.hpp" "tests_cpp14/*.tpp")
	file(GLOB_RECURSE TEST_SOURCES_CPP17 "tests_cpp17/*.cpp" "tests_cpp17/*.hpp" "tests_cpp17/*.tpp")
	add_executable(${TEST_NAME} ${TEST_SOURCES_CPP14}
		"$<$<COMPILE_FEATURES:cxx_std_17>:${TEST_SOURCES_CPP17}>"
	)
	set_compile_options(${TEST_NAME} PRIVATE)

	# g++ 8 needs to link with seperate filesystem library.
	target_link_libraries(${TEST_NAME} PRIVATE ${PROJECT_NAME} GTest::GTest TBB::TBB
		$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},9.0>>:stdc++fs>
	)

	make_pretty_source_folders("tests_cpp14" "Source Files" "${TEST_SOURCES_CPP14}")
	make_pretty_source_folders("tests_cpp17" "Source Files" "${TEST_SOURCES_CPP17}")

	if (${FEA_BENCHMARKS})
		target_compile_definitions(${TEST_NAME} PRIVATE FEA_BENCHMARKS)
	endif()

	gtest_discover_tests(${TEST_NAME})
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
