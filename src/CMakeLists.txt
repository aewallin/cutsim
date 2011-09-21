project(cutsim)

cmake_minimum_required(VERSION 2.4)

#
# Turn compiler warnings up to 11, at least with gcc.  I dont know how to
# do this with other compilers we might support and I'm leaving it up to
# the relevant platform maintainers...
# #include'ing the boost graph-library creates deprecated warnings
# thus we use now use -Wno-deprecated here.
#
if (CMAKE_BUILD_TOOL MATCHES "make")
    MESSAGE(STATUS "setting gcc options: -Wall -Werror -Wno-deprecated -pedantic-errors")
    add_definitions(-Wall  -Wno-deprecated )
    # qt does not like this: -Werror -pedantic-errors
endif (CMAKE_BUILD_TOOL MATCHES "make")

option(BUILD_TYPE
  "Build type: Release=ON/Debug=OFF  " ON)

if (BUILD_TYPE)
    MESSAGE(STATUS " CMAKE_BUILD_TYPE = Release")
    set(CMAKE_BUILD_TYPE Release)
endif(BUILD_TYPE)

if (NOT BUILD_TYPE)
    MESSAGE(STATUS " CMAKE_BUILD_TYPE = Debug")
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT BUILD_TYPE)

# find out the number of CPUs
# from: http://www.kitware.com/blog/home/post/63
if(NOT DEFINED PROCESSOR_COUNT)
  # Unknown:
  set(PROCESSOR_COUNT 0)
  # Linux:
  set(cpuinfo_file "/proc/cpuinfo")
  if(EXISTS "${cpuinfo_file}")
    file(STRINGS "${cpuinfo_file}" procs REGEX "^processor.: [0-9]+$")
    list(LENGTH procs PROCESSOR_COUNT)
  endif()
  MESSAGE(STATUS "Number of processors on this system " ${PROCESSOR_COUNT} )
  # FIXME: how do we have cmake/make use all the processors?
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})


# find BOOST and boost-python
find_package( Boost   REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    MESSAGE(STATUS "found Boost: " ${Boost_LIB_VERSION})
    MESSAGE(STATUS "boost-incude dirs are: " ${Boost_INCLUDE_DIRS})
    #MESSAGE(STATUS "boost-python lib is: " ${Boost_PYTHON_LIBRARY})
    MESSAGE(STATUS "boost_LIBRARY_DIRS is: " ${Boost_LIBRARY_DIRS})
    MESSAGE(STATUS "Boost_LIBRARIES is: " ${Boost_LIBRARIES})    
endif()

#
# find OpenMP
#

find_package( OpenMP REQUIRED )

IF (OPENMP_FOUND)
    MESSAGE(STATUS "found OpenMP, compiling with flags: " ${OpenMP_CXX_FLAGS} )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF(OPENMP_FOUND)

#
# for cutsim, find OpenGL and Qt.
#

MESSAGE(STATUS "Cutting simulation requires OpenGL and Qt4...")
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
    MESSAGE(STATUS "found OPENGL, lib = " ${OPENGL_LIBRARIES} )
endif(OPENGL_FOUND)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtXml QtOpenGL REQUIRED)
message(STATUS " qmake = ${QT_QMAKE_EXECUTABLE}") 
INCLUDE(${QT_USE_FILE})
MESSAGE(STATUS "QT_USE_FILE = " ${QT_USE_FILE} )

set (MOC_HEADERS  # these headers need Qt MOCing
    ${cutsim_SOURCE_DIR}/glwidget.hpp
    ${cutsim_SOURCE_DIR}/cutsim.hpp 
)
    
# this defines the source-files

set( CUTSIM_SRC
    ${cutsim_SOURCE_DIR}/volume.cpp
    ${cutsim_SOURCE_DIR}/octnode.cpp
    ${cutsim_SOURCE_DIR}/octree.cpp
    ${cutsim_SOURCE_DIR}/marching_cubes.cpp
    ${cutsim_SOURCE_DIR}/glwidget.cpp 
    ${cutsim_SOURCE_DIR}/gldata.cpp 
    ${cutsim_SOURCE_DIR}/bbox.cpp 
    ${cutsim_SOURCE_DIR}/cutsim.cpp 
)

set( CUTSIM_INCLUDE_FILES
    ${cutsim_SOURCE_DIR}/octnode.hpp
    ${cutsim_SOURCE_DIR}/octree.hpp
    ${cutsim_SOURCE_DIR}/volume.hpp
    ${cutsim_SOURCE_DIR}/bbox.hpp
    ${cutsim_SOURCE_DIR}/marching_cubes.hpp
    ${cutsim_SOURCE_DIR}/gldata.hpp
    ${cutsim_SOURCE_DIR}/glvertex.hpp
    ${cutsim_SOURCE_DIR}/glwidget.hpp
    ${cutsim_SOURCE_DIR}/cutsim.hpp 
)


# include dirs
include_directories( ${cutsim_SOURCE_DIR} )


# this is the cutting sim
qt4_wrap_cpp(MOC_OUTFILES ${MOC_HEADERS})
add_library(cutsim SHARED 
    ${CUTSIM_SRC}
    ${MOC_OUTFILES}
)
target_link_libraries(cutsim  ${OPENGL_LIBRARIES})


install(
    TARGETS cutsim
    LIBRARY 
    DESTINATION lib/cutsim
    ARCHIVE DESTINATION lib/cutsim
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# this installs the c++ include headers
install(
    FILES ${CUTSIM_INCLUDE_FILES}
    DESTINATION include/cutsim
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)


# "make package" stuff 
execute_process(
    COMMAND dpkg --print-architecture
    OUTPUT_VARIABLE DEB_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "opencamlib@googlegroups.com")
set(CPACK_PACKAGE_NAME "cutsim")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "cutting simulation library")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_VERSION "")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libgomp1" )
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${DEB_ARCHITECTURE})
SET(CPACK_PACKAGE_VERSION_MAJOR "11")
SET(CPACK_PACKAGE_VERSION_MINOR "09")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

include(CPack) # this should be last
