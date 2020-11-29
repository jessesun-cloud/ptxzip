cmake_minimum_required(VERSION 3.9)

#ptxzip application
#InitQt("Core;Widgets;LinguistTools;OpenGL")
set(Support_E57 OFF CACHE BOOL "Read from E57 format")

set(PUBLIC_HEADERS 
  inc/PtxReader.hpp
  inc/ptxWriter.hpp 
)
set(PROJECT_SRCS
  PtxReader.cpp
  ptxWriter.cpp
)

find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
set(PUBLIC_HEADER_PATH "${CMAKE_SOURCE_DIR}/inc")


#project ptxlib
set(PtxLib "ptxlib")
add_library(${PtxLib} STATIC "${PROJECT_SRCS}" "${PUBLIC_HEADERS}")
target_include_directories(${PtxLib} PUBLIC ${PUBLIC_HEADER_PATH})
target_include_directories(${PtxLib} PUBLIC ${CMAKE_SOURCE_DIR})
AddExternalDependency(${PtxLib} PRIVATE Boost)
set_target_properties(${PtxLib} PROPERTIES FOLDER ${PtxLib})
source_group("Source Files" FILES ${PROJECT_SRCS})
source_group("Public Headers" FILES ${PUBLIC_HEADERS})
##############################################################################################


#SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /FORCE")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")

set(APP "ptxzip")
add_executable(${APP} WIN32  main.cpp)
set_target_properties(${APP} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
target_link_libraries(${APP} PUBLIC ${PtxLib})

#target_include_directories(ptxzip PUBLIC ${Qt5Widgets_INCLUDES})
#target_link_libraries(ptxzip PRIVATE Qt5::Widgets)
AddExternalDependency(ptxzip PRIVATE Boost)
set_target_properties(${APP} PROPERTIES FOLDER ${APP})


##########################
if (Support_E57)
	set(APP "e57toPtx")
	add_definitions(-DSUPPORT_E57)
	add_executable(${APP} WIN32  e57toPtx.cpp)
	set_target_properties(${APP} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	target_link_libraries(${APP} PUBLIC E57Format)
	target_link_libraries(${APP} PUBLIC ${PtxLib})
	AddExternalDependency(${APP} PRIVATE Boost)
	AddExternalDependency(${APP} PRIVATE Eigen)	
	#AddExternalDependency(${APP} PUBLIC glog)
	#AddExternalDependency(${APP} PUBLIC OpenCV)
	set_target_properties(${APP} PROPERTIES FOLDER ${APP})
endif()