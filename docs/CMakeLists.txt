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
  
if (SUPPORTS_OPEN_CL_2_0_APIS OR SUPPORTS_OPEN_CL_2_1_APIS OR SUPPORTS_OPEN_CL_2_2_APIS)
  add_dependencies(docs generate_cl2hpp)
else ()
  add_dependencies(docs generate_clhpp)
endif()

endif (DOXYGEN_FOUND)
