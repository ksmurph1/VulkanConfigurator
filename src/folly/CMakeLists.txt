cmake_minimum_required(VERSION 3.12.4 FATAL_ERROR)

# We use the GoogleTest module if it is available (only in CMake 3.9+)
# It requires CMP0054 and CMP0057 to be enabled.
if (POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()
if (POLICY CMP0057)
  cmake_policy(SET CMP0057 NEW)
endif()

# CMP0075 Include file check macros honor CMAKE_REQUIRED_LIBRARIES
if(POLICY CMP0075)
  cmake_policy(SET CMP0075 NEW)
endif()

# includes
set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH})
# package information
set(PACKAGE_NAME      "folly")
set(PACKAGE_VERSION   "0.58.0-dev")
set(PACKAGE_STRING    "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME   "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/facebook/folly/issues")

# 150+ tests in the root folder anyone? No? I didn't think so.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(${PACKAGE_NAME} CXX C)

file(GLOB SOURCES LIST_DIRECTORIES true ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(INCLUDE_INSTALL_DIR include CACHE STRING
    "The subdirectory where header files should be installed")
set(LIB_INSTALL_DIR lib CACHE STRING
    "The subdirectory where libraries should be installed")
set(BIN_INSTALL_DIR bin CACHE STRING
    "The subdirectory where binaries should be installed")
set(CMAKE_INSTALL_DIR lib/cmake/folly CACHE STRING
    "The subdirectory where CMake package config files should be installed")

option(BUILD_SHARED_LIBS
  "If enabled, build folly as a shared library.  \
  This is generally discouraged, since folly does not commit to having \
  a stable ABI."
  OFF
)
# Mark BUILD_SHARED_LIBS as an "advanced" option, since enabling it
# is generally discouraged.
mark_as_advanced(BUILD_SHARED_LIBS)
set(FOLLY_SUPPORT_SHARED_LIBRARY "${BUILD_SHARED_LIBS}")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Check target architecture
  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Folly requires a 64bit target architecture.")
  endif()

  if (MSVC_VERSION LESS 1900)
    message(
      FATAL_ERROR
      "This build script only supports building Folly on 64-bit Windows with "
      "at least Visual Studio 2017. "
      "MSVC version '${MSVC_VERSION}' is not supported."
    )
  endif()
endif()

set(TOP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(FOLLY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/folly")
set(
  FOLLY_DIR_PREFIXES
  "${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_CURRENT_BINARY_DIR}"
)

include(GNUInstallDirs)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(FOLLY_HAVE_PTHREAD "${CMAKE_USE_PTHREADS_INIT}")
list(APPEND CMAKE_REQUIRED_LIBRARIES Threads::Threads)
list(APPEND FOLLY_LINK_LIBRARIES Threads::Threads)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  include(FollyCompilerMSVC)
else()
  include(FollyCompilerUnix)
endif()
include(FollyFunctions)

include(folly-deps) # Find the required packages
include(FollyConfigChecks)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake/folly-config.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/folly/folly-config.h
)

# We currently build the main libfolly library by finding all sources
# and header files.  We then exclude specific files below.
#
# In the future it would perhaps be nicer to explicitly list the files we want
# to include, and to move the source lists in to separate per-subdirectory
# CMakeLists.txt files.
auto_sources(files "*.cpp" "RECURSE" "${FOLLY_DIR}")
auto_sources(hfiles "*.h" "RECURSE" "${FOLLY_DIR}")

# Exclude tests, benchmarks, and other standalone utility executables from the
# library sources.  Test sources are listed separately below.
REMOVE_MATCHES_FROM_LISTS(files hfiles
  MATCHES
    "^${FOLLY_DIR}/build/"
)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  list(REMOVE_ITEM files
    ${FOLLY_DIR}/Poly.cpp
    ${FOLLY_DIR}/Subprocess.cpp
  )
  list(REMOVE_ITEM hfiles
    ${FOLLY_DIR}/Poly.h
    ${FOLLY_DIR}/Poly-inl.h
    ${FOLLY_DIR}/detail/PolyDetail.h
    ${FOLLY_DIR}/detail/TypeList.h
    ${FOLLY_DIR}/poly/Nullable.h
    ${FOLLY_DIR}/poly/Regular.h
  )
endif()

add_library(folly_base OBJECT
  ${files} ${hfiles}
  ${CMAKE_CURRENT_BINARY_DIR}/folly/folly-config.h
)

auto_source_group(folly ${FOLLY_DIR} ${files} ${hfiles})
apply_folly_compile_options_to_target(folly_base)
# Add the generated files to the correct source group.
source_group("folly" FILES ${CMAKE_CURRENT_BINARY_DIR}/folly/folly-config.h)

# Generate pkg-config variables from folly_deps before we add our own
# build/install-time include directory generator expressions
include(GenPkgConfig)
gen_pkgconfig_vars(FOLLY_PKGCONFIG folly_deps)

target_include_directories(folly_deps
  BEFORE
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_include_directories(folly_deps
  INTERFACE
    $<INSTALL_INTERFACE:include>
)

target_include_directories(folly_base
  PUBLIC
    $<TARGET_PROPERTY:folly_deps,INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(folly_base
  PUBLIC
    $<TARGET_PROPERTY:folly_deps,INTERFACE_COMPILE_DEFINITIONS>
)

set(FOLLY_INSTALL_TARGETS folly folly_deps)

add_library(folly
  $<TARGET_OBJECTS:folly_base> ${SOURCES}
)
#set_target_properties(${FOLLY_INSTALL_TARGETS} PROPERTIES LINKER_LANGUAGE CXX)
apply_folly_compile_options_to_target(folly)
target_compile_features(folly INTERFACE cxx_generic_lambdas)

target_link_libraries(folly PUBLIC folly_deps)

# set_target_properties(TARGETS PROPERTIES LINKER_LANGUAGE CXX)
install(TARGETS ${FOLLY_INSTALL_TARGETS}
  EXPORT folly
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
auto_install_files(folly ${FOLLY_DIR}
  ${hfiles}
)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/folly/folly-config.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/folly
  COMPONENT dev
)

# Generate the folly-config.cmake file for installation so that
# downstream projects that use on folly can easily depend on it in their CMake
# files using "find_package(folly CONFIG)"
include(CMakePackageConfigHelpers)
configure_package_config_file(
  CMake/folly-config.cmake.in
  folly-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DIR}
  PATH_VARS
    INCLUDE_INSTALL_DIR
    CMAKE_INSTALL_DIR
)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/folly-config.cmake
  DESTINATION ${CMAKE_INSTALL_DIR}
  COMPONENT dev
)
install(
  EXPORT folly
  DESTINATION ${CMAKE_INSTALL_DIR}
  NAMESPACE Folly::
  FILE folly-targets.cmake
  COMPONENT dev
)

# Generate a pkg-config file so that downstream projects that don't use
# CMake can depend on folly using pkg-config.
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake/libfolly.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/libfolly.pc.gen
  @ONLY
)
#file(
#  GENERATE
#  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libfolly.pc
#  INPUT ${CMAKE_CURRENT_BINARY_DIR}/libfolly.pc.gen
#)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libfolly.pc
  DESTINATION ${LIB_INSTALL_DIR}/pkgconfig
  COMPONENT dev
)
# The ThreadLocalTest code uses a helper shared library for one of its tests.
  # This can only be built if folly itself was built as a shared library.
  if (BUILD_SHARED_LIBS)
    add_library(thread_local_test_lib MODULE
      ${FOLLY_DIR}/test/ThreadLocalTestLib.cpp
    )
    set_target_properties(thread_local_test_lib PROPERTIES PREFIX "")
    apply_folly_compile_options_to_target(thread_local_test_lib)
    target_link_libraries(thread_local_test_lib PUBLIC folly)
    target_include_directories(
      thread_local_test_lib
      PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
  endif()
add_subdirectory(src)
