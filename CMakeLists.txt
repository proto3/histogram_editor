cmake_minimum_required(VERSION 2.8)
project(bench)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

################################################################################
#PACKAGES
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)

################################################################################
#INCLUDES
include_directories(
    include
    ${Boost_INCLUDE_DIRS}
)

################################################################################
#SOURCES
file(GLOB_RECURSE HISTOGRAM_EDITOR_SRC "src/HistogramEditor/*.cpp")

################################################################################
#LINK
LIST(APPEND LINK_LIBS
   ${OpenCV_LIBS}
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_FILESYSTEM_LIBRARY}
)

################################################################################
#EXECUTABLE
add_executable(${PROJECT_NAME} ${HISTOGRAM_EDITOR_SRC} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})
