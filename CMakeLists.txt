cmake_minimum_required(VERSION 3.0)
project(EasyCppOGL)

option(BUILD_TP_Prog_Graphique_3D "Build TP_Prog_Graphique_3D." ON)
if (WIN32)
	set (SYS_DATA_PATH $ENV{HOME} CACHE PATH "Path where to read image mesh ...")
else()
	set (SYS_DATA_PATH $ENV{HOMEPATH} CACHE PATH "Path where to read image mesh ...")
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
set(CMAKE_DEBUG_POSTFIX "_d")

add_definitions("-DDEF_DATA_PATH=${SYS_DATA_PATH}")

add_subdirectory(easycppogl_src)

if (${BUILD_TP_Prog_Graphique_3D})
	add_subdirectory(TP_Prog_Graphique_3D)
endif()
