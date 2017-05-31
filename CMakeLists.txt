cmake_minimum_required(VERSION 2.8)
project(histogram_editor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

################################################################################
#PACKAGES
find_package(OpenCV REQUIRED)

################################################################################
#INCLUDES
include_directories(
    include
)

################################################################################
#SOURCES
file(GLOB_RECURSE HISTOGRAM_EDITOR_SRC "src/HistogramEditor/*.cpp")

################################################################################
#LINK
LIST(APPEND LINK_LIBS
   ${OpenCV_LIBS}
)

################################################################################
#EXECUTABLE
add_executable(${PROJECT_NAME} ${HISTOGRAM_EDITOR_SRC} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})
