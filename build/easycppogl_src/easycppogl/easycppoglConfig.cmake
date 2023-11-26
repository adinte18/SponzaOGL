
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was easycppoglConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

####################################################################################
include(CMakeFindDependencyMacro)
find_dependency(glfw3 REQUIRED)
find_dependency(assimp REQUIRED)
find_dependency(Eigen3 REQUIRED)

if(NOT TARGET easycppogl)
	include("${CMAKE_CURRENT_LIST_DIR}/easycppoglTargets.cmake")
endif()
