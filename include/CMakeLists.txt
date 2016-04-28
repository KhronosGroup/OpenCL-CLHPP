add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CL/cl.hpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/CL
    COMMAND python ${CLCPP_SOURCE_DIR}/gen_cl_hpp.py -i ${CLCPP_SOURCE_DIR}/input_cl.hpp -o ${CMAKE_CURRENT_BINARY_DIR}/CL/cl.hpp
    DEPENDS ${CLCPP_SOURCE_DIR}/input_cl.hpp ${CLCPP_SOURCE_DIR}/gen_cl_hpp.py
    COMMENT "Rebuilding cl.hpp ...")
add_custom_target(generate_clhpp DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/CL/cl.hpp SOURCES ${CLCPP_SOURCE_DIR}/input_cl.hpp)

if (SUPPORTS_OPEN_CL_2_0_APIS OR SUPPORTS_OPEN_CL_2_1_APIS OR SUPPORTS_OPEN_CL_2_2_APIS)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CL/cl2.hpp
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/CL
        COMMAND cp ${CLCPP_SOURCE_DIR}/input_cl2.hpp ${CMAKE_CURRENT_BINARY_DIR}/CL/cl2.hpp
        DEPENDS ${CLCPP_SOURCE_DIR}/input_cl2.hpp
        COMMENT "Rebuilding cl2.hpp ...")
    add_custom_target(generate_cl2hpp DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/CL/cl2.hpp SOURCES ${CLCPP_SOURCE_DIR}/input_cl2.hpp)
endif()
