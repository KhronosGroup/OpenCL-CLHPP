file(GLOB OPENCL_LIBS_IN_LIB_DIR "${OPENCL_CLHPP_LOADER_DIR}/*OpenCL*")
if(NOT "${OPENCL_LIBS_IN_LIB_DIR}" STREQUAL "")
  if(NOT TARGET OpenCL)
    message(STATUS "Adding flags to CLHPP examples as per OPENCL_LIB_DIR")
    link_directories("${OPENCL_LIB_DIR}")
  else()
    message(WARNING "User-provided OpenCL library found in ${OPENCL_LIB_DIR}. Cannot use it because the target \"OpenCL\" already exists in the build.")
  endif()
else()
  if(NOT TARGET OpenCL::OpenCL)
    find_package(OpenCLICDLoader REQUIRED)
  endif()
endif()

foreach(EXAMPLE IN ITEMS
  trivial
  trivialSizeTCompat
  headerexample
)
  add_executable(${EXAMPLE} ${EXAMPLE}.cpp)
  target_link_libraries(${EXAMPLE}
    PRIVATE
      OpenCL::HeadersCpp
      OpenCL::Headers
      Threads::Threads
  )
  if(NOT "${OPENCL_LIBS_IN_LIB_DIR}" STREQUAL "")
    target_link_libraries(${EXAMPLE} PRIVATE OpenCL)
  else()
  target_link_libraries(${EXAMPLE} PRIVATE OpenCL::OpenCL)
  endif()
endforeach()
