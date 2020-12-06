cmake_minimum_required(VERSION 3.9)

#InitQt("Core;Widgets;LinguistTools;OpenGL")

set(PUBLIC_HEADERS 
  inc/PtxReader.hpp
  inc/ptxWriter.hpp 
  inc/IScanInterface.hpp
)
set(PROJECT_SRCS
  PtxReader.cpp
  ptxWriter.cpp
  ScanNode.cpp
)

find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
set(PUBLIC_HEADER_PATH "${CMAKE_SOURCE_DIR}/ptxzip/inc")


#project ptxlib
set(PtxLib "ptxlib")
add_library(${PtxLib} STATIC "${PROJECT_SRCS}" "${PUBLIC_HEADERS}")
target_link_libraries(${PtxLib}  Eigen)
target_include_directories(${PtxLib} PUBLIC ${PUBLIC_HEADER_PATH})
target_include_directories(${PtxLib} PUBLIC ${CMAKE_SOURCE_DIR})
set_target_properties(${PtxLib} PROPERTIES FOLDER ${PtxLib})
source_group("Source Files" FILES ${PROJECT_SRCS})
source_group("Public Headers" FILES ${PUBLIC_HEADERS})
##############################################################################################


SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
set(APP "ptxzip")
add_executable(${APP} main.cpp)
target_link_libraries(${APP} PUBLIC ${PtxLib})

#target_include_directories(ptxzip PUBLIC ${Qt5Widgets_INCLUDES})
#target_link_libraries(ptxzip PRIVATE Qt5::Widgets)
#AddExternalDependency(${APP} PRIVATE Boost)
set_target_properties(${APP} PROPERTIES FOLDER ${PtxLib})
