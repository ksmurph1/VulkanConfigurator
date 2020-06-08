cmake_minimum_required(VERSION 3.12.4)

set(BINARY ${CMAKE_PROJECT_NAME})
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.hpp *.cpp)
list(REMOVE_ITEM SOURCES folly/*.h folly/*.hpp folly/*.cpp main.cpp)

#set(SOURCES ${SOURCES})
#add_executable(${BINARY}_run ${SOURCES})
add_library(${BINARY}_lib STATIC ${SOURCES})

find_package(PkgConfig)
pkg_check_modules(PC_TINYXML2 QUIET tinyxml2)
set(TINYXML2_DEFINITIONS ${PC_TINYXML2_CFLAGS_OTHER})

find_path(TINYXML2_INCLUDE_DIR tinyxml2.h
          HINTS ${PC_TINYXML2_INCLUDEDIR} ${PC_TINYXML2_INCLUDE_DIRS}
          PATH_SUFFIXES tinyxml2 )

find_library(TINYXML2_LIBRARY NAMES tinyxml2 libtinyxml2
             HINTS ${PC_TINYXML2_LIBDIR} ${PC_TINYXML2_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set TINYXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(tinyxml2  DEFAULT_MSG
                                  TINYXML2_LIBRARY TINYXML2_INCLUDE_DIR)

mark_as_advanced(TINYXML2_INCLUDE_DIR TINYXML2_LIBRARY )

set(TINYXML2_LIBRARIES ${TINYXML2_LIBRARY} )
set(TINYXML2_INCLUDE_DIRS ${TINYXML2_INCLUDE_DIR} )

if (TINYXML2_FOUND)
   include_directories(${TINYXML2_INCLUDE_DIRS})
   target_link_libraries(${BINARY}_lib ${TINYXML2_LIBRARIES})
   message ("-- tinyxml2 found:  ${TINYXML2_VERSION_STRING}")
else()
   message(FATAL_ERROR "tinyxml2 not found!")
endif()

# jemalloc
pkg_check_modules(PC_JEMALLOC QUIET jemalloc)
set(JEMALLOC_DEFINITIONS ${PC_JEMALLOC_CFLAGS_OTHER})

find_path(JEMALLOC_INCLUDE_DIR jemalloc/jemalloc.h
          HINTS ${PC_JEMALLOC_INCLUDEDIR} ${PC_JEMALLOC_INCLUDE_DIRS}
          PATH_SUFFIXES jemalloc )

find_library(JEMALLOC_LIBRARY NAMES jemalloc libjemalloc
             HINTS ${PC_JEMALLOC_LIBDIR} ${PC_JEMALLOC_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set JEMALLOC_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(JEMALLOC  DEFAULT_MSG JEMALLOC_LIBRARY JEMALLOC_INCLUDE_DIR)

mark_as_advanced(JEMALLOC_INCLUDE_DIR JEMALLOC_LIBRARY )

set(JEMALLOC_LIBRARIES ${JEMALLOC_LIBRARY} )
set(JEMALLOC_INCLUDE_DIRS ${JEMALLOC_INCLUDE_DIR} )

if (JEMALLOC_FOUND)
   include_directories(${JEMALLOC_INCLUDE_DIRS})
   target_link_libraries(${BINARY}_lib ${JEMALLOC_LIBRARIES})
   message ("-- jemalloc found:  ${JEMALLOC_VERSION_STRING}")
else()
   message(FATAL_ERROR "jemalloc not found!")
endif()

# vulkan
pkg_check_modules(PC_VULKAN QUIET vulkan)
set(VULKAN_DEFINITIONS ${PC_VULKAN_CFLAGS_OTHER})

find_path(VULKAN_INCLUDE_DIR vulkan/vulkan.h
          HINTS ${PC_VULKAN_INCLUDEDIR} ${PC_VULKAN_INCLUDE_DIRS}
          PATH_SUFFIXES vulkan )

find_library(VULKAN_LIBRARY NAMES vulkan libvulkan
             HINTS ${PC_VULKAN_LIBDIR} ${PC_VULKAN_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set VULKAN_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(VULKAN DEFAULT_MSG VULKAN_LIBRARY VULKAN_INCLUDE_DIR)

mark_as_advanced(VULKAN_INCLUDE_DIR VULKAN_LIBRARY )

set(VULKAN_LIBRARIES ${VULKAN_LIBRARY} )
set(VULKAN_INCLUDE_DIRS ${VULKAN_INCLUDE_DIR} )

if (VULKAN_FOUND)
   include_directories(${VULKAN_INCLUDE_DIRS})
   target_link_libraries(${BINARY}_lib ${VULKAN_LIBRARIES})
   message ("-- vulkan found:  ${VULKAN_VERSION_STRING}")
else()
   message(FATAL_ERROR "vulkan not found!")
endif()

# xcb
pkg_check_modules(PC_XCB QUIET xcb)
set(XCB_DEFINITIONS ${PC_XCB_CFLAGS_OTHER})

find_path(XCB_INCLUDE_DIR xcb/xcb.h
          HINTS ${PC_XCB_INCLUDEDIR} ${PC_XCB_INCLUDE_DIRS}
          PATH_SUFFIXES xcb )

find_library(XCB_LIBRARY NAMES xcb libxcb
             HINTS ${PC_XCB_LIBDIR} ${PC_XCB_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set XCB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(XCB DEFAULT_MSG XCB_LIBRARY XCB_INCLUDE_DIR)

mark_as_advanced(XCB_INCLUDE_DIR XCB_LIBRARY )

set(XCB_LIBRARIES ${XCB_LIBRARY} )
set(XCB_INCLUDE_DIRS ${XCB_INCLUDE_DIR} )

if (XCB_FOUND)
   include_directories(${XCB_INCLUDE_DIRS})
   target_link_libraries(${BINARY}_lib ${XCB_LIBRARIES})
   message ("-- xcb found:  ${XCB_VERSION_STRING}")
else()
   message(FATAL_ERROR "xcb not found!")
endif()
target_link_libraries(${BINARY}_lib folly)
#target_link_libraries(${BINARY}_lib ${CMAKE_PROJECT_NAME}_tst gtest)
#target_link_libraries(${BINARY}_run ${BINARY}_lib)
#target_link_libraries(${BINARY} PUBLIC ${BINARY}_lib)
#target_link_libraries(${BINARY}_lib -L/home/kerry/vulkan/src/folly/build/)
#target_link_libraries(${BINARY}_lib -lfolly)
#target_link_libraries(${BINARY}_lib -lvulkan)
#target_link_libraries(${BINARY}_lib -lxcb)
#target_link_libraries(${BINARY}_lib -ljemalloc)
#target_link_libraries(${BINARY}_lib -ltinyxml2)
#add_subdirectory(folly/build)