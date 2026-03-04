cmake_minimum_required(VERSION 2.8...3.27.6)


PROJECT(vtktp)

add_definitions(-std=c++17)
set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
set(CMAKE_BUILD_TYPE Debug)


set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
set(CMAKE_XCODE_SCHEME_WORKING_DIRECTORY)

IF(APPLE)

SET( OperatingSystem "Mac os X")
set (MY_DATA_PATH  /Users/p4179/shared_folders/data_VTK)


find_package(VTK 9 REQUIRED)

ENDIF (APPLE)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

SET(OperatingSystem "Linux")

set (MY_DATA_PATH  /tp)

find_package(VTK)
include(${VTK_USE_FILE})

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


MESSAGE(WARNING "System detected:${OperatingSystem}" )
MESSAGE(WARNING "Path to data: ${MY_DATA_PATH}" )


configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
 

file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )

include_directories("${PROJECT_BINARY_DIR}")


 
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})



add_executable(${PROJECT_NAME} ${SRC_LIST})

 
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES} ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()



set(listnb 1 2 4 8 )

if(CMAKE_GENERATOR STREQUAL Xcode)
message(STATUS "Xcode detected ")

set_property (TARGET ${PROJECT_NAME} PROPERTY XCODE_SCHEME_ARGUMENTS "--oversubscribe  -np 2"  ${PROJECT_BINARY_DIR}/Debug/${PROJECT_NAME}  )

set_property (TARGET ${PROJECT_NAME} PROPERTY XCODE_SCHEME_EXECUTABLE "/usr/local/bin/mpirun" )


set_property (TARGET ${PROJECT_NAME} PROPERTY XCODE_SCHEME_WORKING_DIRECTORY ${PROJECT_BINARY_DIR} )

foreach(X IN LISTS listnb )
    
message(STATUS " set prunx${X}")
add_custom_target(prunx${X}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
set(nbnodes "${X}") 
set_property (TARGET prunx${X} PROPERTY XCODE_SCHEME_ARGUMENTS "--oversubscribe  -np " ${nbnodes} ${PROJECT_BINARY_DIR}/Debug/${PROJECT_NAME}  )
set_property (TARGET prunx${X} PROPERTY XCODE_SCHEME_EXECUTABLE "/usr/local/bin/mpirun" )
set_property (TARGET prunx${X} PROPERTY XCODE_SCHEME_WORKING_DIRECTORY ${PROJECT_BINARY_DIR} )
endforeach()


else()

foreach(X IN LISTS listnb )
message(STATUS " set prun${X}")

add_custom_target(prun${X}
    COMMAND mpirun  --oversubscribe  -np ${X} ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
endforeach()

endif()


enable_testing()
add_test(SimpleTest ${PROJECT_NAME})
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS ${PROJECT_NAME})
