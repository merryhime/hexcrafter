add_library(catch INTERFACE)
target_include_directories(catch INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/catch>)

add_library(gsl INTERFACE)
target_include_directories(gsl INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/GSL/include>)
