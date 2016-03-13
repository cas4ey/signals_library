cmake_minimum_required(VERSION 2.4)
project( signals_test )

set(ROOT
 ${CMAKE_CURRENT_LIST_DIR}
)

include_directories(
  ${ROOT}/include
  ${ROOT}/include/3rdparty
)

set(OUTPUT_DIR
  ${ROOT}/bin
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
  ${OUTPUT_DIR}
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${OUTPUT_DIR}
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${OUTPUT_DIR}
)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic" )
else()
  add_definitions( -D_CRT_SECURE_NO_WARNINGS )
endif(UNIX)

add_subdirectory(shared_allocator)
add_subdirectory(test)
