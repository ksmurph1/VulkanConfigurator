set(BINARY ${CMAKE_PROJECT_NAME}_tst)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES true *.h *.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})

add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} PUBLIC gtest)
target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib)

file (GLOB_RECURSE xmltests "*.xml")

foreach(resource ${xmltests})

   get_filename_component(filename ${resource} NAME)
   set(destination "${CMAKE_BINARY_DIR}/unittests/${filename}")
   add_custom_command(
   COMMENT "Copying updated resource-file '${filename}'"
   OUTPUT ${destination}
   DEPENDS ${resource}
   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${resource} ${destination}
   )
   add_custom_target(${filename} ALL DEPENDS ${resource} ${destination})
endforeach()