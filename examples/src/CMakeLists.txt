if( MSVC )
  set(CMAKE_CXX_FLAGS " -W3 /EHsc" )
elseif( CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "-Wall" )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
  set(CMAKE_CXX_FLAGS "-Wall" )
endif()

foreach(TEST IN ITEMS
  trivial
  trivialSizeTCompat
  headerexample
)
  add_executable(${TEST}
    ${TEST}.cpp
    ${PROJECT_SOURCE_DIR}/include/CL/cl2.hpp
  )
  target_include_directories(${TEST}
    PRIVATE
      ${PROJECT_SOURCE_DIR}/include
  )
  target_link_libraries(${TEST}
    PRIVATE
      OpenCL::Headers
      OpenCL::ICDLoader
  )
endforeach()
