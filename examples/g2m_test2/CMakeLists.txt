project(g2m_example2)

cmake_minimum_required(VERSION 2.4)

if (CMAKE_BUILD_TOOL MATCHES "make")
    add_definitions(-Wall  -Wno-deprecated )
endif (CMAKE_BUILD_TOOL MATCHES "make")
# -Werror
# -pedantic-errors

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtXml QtOpenGL REQUIRED)
INCLUDE(${QT_USE_FILE})
 MESSAGE(STATUS "QT_USE_FILE = " ${QT_USE_FILE} )

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
    MESSAGE(STATUS "found OPENGL, lib = " ${OPENGL_LIBRARIES} )
endif(OPENGL_FOUND)


# find BOOST and boost-python
find_package( Boost )
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    MESSAGE(STATUS "found Boost: " ${Boost_LIB_VERSION})
    MESSAGE(STATUS "boost-incude dirs are: " ${Boost_INCLUDE_DIRS})
endif()


find_library(CUTSIM_LIBRARY 
            NAMES cutsim
            PATHS /usr/local/lib/cutsim /usr/lib/cutsim
            DOC "The cutsim library"
)
MESSAGE(STATUS "CUTSIM_LIBRARY is now: " ${CUTSIM_LIBRARY})

find_library(G2M_LIBRARY 
            NAMES g2m
            PATHS /usr/local/lib/g2m /usr/lib/g2m
            DOC "The g2m library"
)
MESSAGE(STATUS "G2M_LIBRARY is now: " ${G2M_LIBRARY})

#set (MOC_HEADERS cutsim.hpp )
#qt4_wrap_cpp(MOC_OUTFILES ${MOC_HEADERS})

set( g2m_tst_SRC 
     main.cpp 
    )

add_executable( 
    ${PROJECT_NAME} 
    ${g2m_tst_SRC} 
    )
target_link_libraries( ${PROJECT_NAME} ${G2M_LIBRARY} ${CUTSIM_LIBRARY} ${QT_LIBRARIES} ${Boost_LIBRARIES} ${OPENGL_LIBRARIES})

install( TARGETS ${PROJECT_NAME} DESTINATION bin )
