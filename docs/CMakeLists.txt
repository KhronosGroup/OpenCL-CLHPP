find_package(Doxygen)

if (DOXYGEN_FOUND)

configure_file(Doxyfile.in Doxyfile COPYONLY)
add_custom_target(
  docs
  COMMAND
  ${DOXYGEN_EXECUTABLE}
  WORKING_DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}
  VERBATIM)
add_dependencies(docs generate_cl2hpp)

endif (DOXYGEN_FOUND)
