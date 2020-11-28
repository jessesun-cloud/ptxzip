#ptxzip application
#InitQt("Core;Widgets;LinguistTools;OpenGL")

set(PUBLIC_HEADERS 
  PtxReader.hpp
  ptxWriter.hpp 
)
set(PROJECT_SRCS
  PtxReader.cpp
  ptxWriter.cpp
  main.cpp
)

find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
set(PUBLIC_HEADER_PATH "${CMAKE_SOURCE_DIR}/ptxreader/inc")
PrependPathToFiles("${PUBLIC_HEADER_PATH}" "${PUBLIC_HEADERS}" "PUBLIC_HEADERS")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /FORCE")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
AddExecutable(ptxzip "${PROJECT_SRCS}" "${PUBLIC_HEADERS}")
target_include_directories(ptxzip PUBLIC ${PUBLIC_HEADER_PATH})

#target_include_directories(ptxzip PUBLIC ${Qt5Widgets_INCLUDES})
#target_link_libraries(ptxzip PRIVATE Qt5::Widgets)
#AddExternalDependency(ptxzip PRIVATE Boost)
#AddExternalDependency(ptxzip PRIVATE Eigen)	
#AddExternalDependency(ptxzip PUBLIC glog)
#AddExternalDependency(ptxzip PUBLIC OpenCV)

source_group("Source Files" FILES ${PROJECT_SRCS})
source_group("Public Headers" FILES ${PUBLIC_HEADERS})
set_target_properties(ptxzip PROPERTIES FOLDER "ptxzip")
