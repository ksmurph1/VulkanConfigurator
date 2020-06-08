cmake_minimum_required(VERSION 3.12.4)
project(DynamicVulkanConfigurator)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.70 REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    message("-- Boost found: ${Boost_VERSION_STRING}")
else()
    message(FATAL_ERROR "Boost not found!")
endif()

set(BINARY ${CMAKE_PROJECT_NAME})
#set(SOURCES src/main.cpp)

#add_executable(${BINARY}_run ${SOURCES})

#target_link_libraries(${BINARY}_run PUBLIC ${BINARY}_lib)
include_directories(src)

add_subdirectory(src)
add_subdirectory(unittests)
add_subdirectory(src/folly)
