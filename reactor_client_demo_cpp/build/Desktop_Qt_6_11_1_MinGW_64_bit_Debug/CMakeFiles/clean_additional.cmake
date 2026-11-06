# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\reactor_client_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\reactor_client_autogen.dir\\ParseCache.txt"
  "reactor_client_autogen"
  )
endif()
