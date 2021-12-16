foreach(EXAMPLE IN ITEMS
  trivial
  #trivialSizeTCompat
  headerexample
)
  add_executable(${EXAMPLE} ${EXAMPLE}.cpp)
  target_link_libraries(${EXAMPLE}
    PRIVATE
      OpenCL::HeadersCpp
      OpenCL::Headers
      OpenCL::OpenCL
      Threads::Threads
  )
endforeach()
