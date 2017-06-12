cmake_minimum_required(VERSION 2.8)

# Options. Turn on with 'cmake -DBUILD_TESTS=ON'.
option(BUILD_TESTS "Build all tests." OFF) # Makes boolean 'test' available.

#Turn on with "cmake -DVEC_REPORT=ON"
option(VEC_REPORT "Display compilation vectorization reports." OFF)

# Make PROJECT_SOURCE_DIR, PROJECT_BINARY_DIR, and PROJECT_NAME available.
set(PROJECT_NAME vertica-getdatafromnode)
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "-g -Wall -O3 -march=native -std=c++11 -Wno-unused-value -fPIC")

if(VEC_REPORT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopt-info-vec-all -ftree-vectorizer-verbose=7")
endif()

# Here we say where g++ should look for include files
set(SDK_HOME /opt/vertica/sdk CACHE FILEPATH "Path to the Vertica SDK, by default /opt/vertica/sdk")
if(NOT EXISTS ${SDK_HOME})
  message(FATAL_ERROR "Could not build. No SDK found at ${SDK_HOME} (maybe retry with -DSDK_HOME=<sdk_path>).")
endif()


set(VERTICA_INCLUDE ${SDK_HOME}/include)
include_directories(${VERTICA_INCLUDE} include src)

set(SOURCES src/GetDataFromNode.cpp ${VERTICA_INCLUDE}/Vertica.h ${VERTICA_INCLUDE}/BuildInfo.h ${VERTICA_INCLUDE}/Vertica.cpp)
add_library(getdatafromnode SHARED ${SOURCES})

# Installation process just copies the binary HLL to Vertica lib folder
set(LIB_DIR /opt/vertica/lib)
install(
  TARGETS getdatafromnode
  DESTINATION ${LIB_DIR}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
