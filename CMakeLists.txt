
project( val3dity )

cmake_minimum_required(VERSION 2.6.2)
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER 2.6)
  if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER 2.8.3)
    cmake_policy(VERSION 2.8.4)
  else()
    cmake_policy(VERSION 2.6)
  endif()
endif()

add_definitions(-std=c++11)

set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
 
if ( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )  
endif()

# Boost
find_package( Boost REQUIRED locale)
if ( NOT Boost_FOUND )
  message(SEND_ERROR "3dfier requires the Boost library")
  return()  
endif()
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )



# Creating entries for target: val3dity
# ############################

add_executable( citygmlinfo pugixml.cpp main.cpp )

include_directories( ${Boost_INCLUDE_DIRS} )


# Link the executable to CGAL and third-party libraries
target_link_libraries(citygmlinfo ${BOOST_LIBRARIES} ${Boost_LIBRARIES})

