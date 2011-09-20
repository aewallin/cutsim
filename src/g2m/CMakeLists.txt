project( g2m )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Debug )
add_definitions ( -Wall )

find_package ( Qt4 REQUIRED )

include ( ${QT_USE_FILE} )
include_directories (
    ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
    )
set ( g2m_HDRS
    g2m.hpp
    #canon.hh
    #canonLine.hh
    #canonMotionless.hh
    #canonMotion.hh
    #linearMotion.hh
    #helicalMotion.hh
    #tool.hh
    #machineStatus.hh
    nanotimer.hpp
    )

set ( g2m_SRCS
    g2m.cpp
    #canon.cc
    #canonLine.cc
    #canonMotionless.cc
    #canonMotion.cc
    #linearMotion.cc
    #helicalMotion.cc
    #tool.cc
    #machineStatus.cc
    nanotimer.cpp
 )

#fix vtable error
set ( g2m_MOCS g2m.hpp )
QT4_WRAP_CPP(MOCS ${g2m_MOCS})

add_library ( g2m SHARED ${g2m_SRCS}  ${MOCS}  ) # ${g2m_HDRS} 
target_link_libraries ( g2m  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )

# install the shared library
install(
    TARGETS g2m 
    DESTINATION lib/g2m
    )

# install the headers
# this installs the c++ include headers
install(
    FILES ${g2m_HDRS}
    DESTINATION include/g2m
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)
