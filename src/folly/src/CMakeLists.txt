#if (POLICY CMP0079)
#  cmake_policy(SET CMP0079 NEW)
#endif()

#set(BINARY folly)



#target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib)
#INSTALL( TARGETS ${CMAKE_PROJECT_NAME}_lib EXPORT folly ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
#target_link_libraries(folly PUBLIC folly-deps)
#apply_folly_compile_options_to_target(follybenchmark)
#install(TARGETS ${FOLLY_INSTALL_TARGETS}
#  EXPORT folly
#  RUNTIME DESTINATION ${BIN_INSTALL_DIR}
# LIBRARY DESTINATION ${LIB_INSTALL_DIR}
#  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
#)