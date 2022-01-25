cmake_minimum_required(VERSION 3.0)

if(NOT INPUT OR NOT OUTPUT)
  message(FATAL_ERROR "Usage: cmake -D INPUT=<input> -D OUTPUT=<output> -P strip_defines.cmake")
endif()

file(READ "${INPUT}" INPUT_STRING)

string(REGEX REPLACE
  [[CL_(API_ENTRY|API_SUFFIX|EXT|CALLBACK)[A-Za-z0-9_]*]]
  ""
  TMP_STRING
  "${INPUT_STRING}"
)

string(REGEX REPLACE
  [=[[*]\[\]]=]
  [=[[*][*]]=]
  OUT_STRING
  "${TMP_STRING}"
)

file(WRITE "${OUTPUT}" "${OUT_STRING}")
