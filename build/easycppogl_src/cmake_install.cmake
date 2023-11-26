# Install script for directory: D:/Programare/P3D/easycppogl/easycppogl_src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/EasyCppOGL")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Programare/P3D/easycppogl/build/easycppogl_src/Debug/easycppogl_d.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Programare/P3D/easycppogl/build/easycppogl_src/Release/easycppogl.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Programare/P3D/easycppogl/build/easycppogl_src/MinSizeRel/easycppogl.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Programare/P3D/easycppogl/build/easycppogl_src/RelWithDebInfo/easycppogl.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/easycppogl" TYPE FILE FILES
    "D:/Programare/P3D/easycppogl/easycppogl_src/imconfig.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/imgui.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/imgui_impl_glfw.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/imgui_impl_opengl3.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/imgui_impl_opengl3_loader.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/imgui_internal.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/imstb_rectpack.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/imstb_textedit.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/imstb_truetype.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/portable_file_dialogs.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/gl_eigen.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/ebo.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/vbo.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/vao.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/shader_program.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/transform_feedback.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/texture.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/texture1d.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/texture2d.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/texture3d.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/texturecubemap.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/texture1darray.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/texture2darray.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/texturebuffer.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/fbo.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/camera.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/gl_viewer.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/mframe.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/boundingbox.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/material.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/scenegraph.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/mesh.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/mesh_assimp_importer.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/ubo.h"
    "D:/Programare/P3D/easycppogl/easycppogl_src/ssbo.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/easycppogl" TYPE DIRECTORY FILES "D:/Programare/P3D/easycppogl/GL" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/easycppogl" TYPE DIRECTORY FILES "D:/Programare/P3D/easycppogl/KHR" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/easycppogl/easycppoglTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/easycppogl/easycppoglTargets.cmake"
         "D:/Programare/P3D/easycppogl/build/easycppogl_src/CMakeFiles/Export/642b13b4a7ac1fb8a12fda35d4012234/easycppoglTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/easycppogl/easycppoglTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/easycppogl/easycppoglTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/easycppogl" TYPE FILE FILES "D:/Programare/P3D/easycppogl/build/easycppogl_src/CMakeFiles/Export/642b13b4a7ac1fb8a12fda35d4012234/easycppoglTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/easycppogl" TYPE FILE FILES "D:/Programare/P3D/easycppogl/build/easycppogl_src/CMakeFiles/Export/642b13b4a7ac1fb8a12fda35d4012234/easycppoglTargets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/easycppogl" TYPE FILE FILES "D:/Programare/P3D/easycppogl/build/easycppogl_src/CMakeFiles/Export/642b13b4a7ac1fb8a12fda35d4012234/easycppoglTargets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/easycppogl" TYPE FILE FILES "D:/Programare/P3D/easycppogl/build/easycppogl_src/CMakeFiles/Export/642b13b4a7ac1fb8a12fda35d4012234/easycppoglTargets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/easycppogl" TYPE FILE FILES "D:/Programare/P3D/easycppogl/build/easycppogl_src/CMakeFiles/Export/642b13b4a7ac1fb8a12fda35d4012234/easycppoglTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/easycppogl" TYPE FILE FILES
    "D:/Programare/P3D/easycppogl/build/easycppogl_src/easycppogl/easycppoglConfig.cmake"
    "D:/Programare/P3D/easycppogl/build/easycppogl_src/easycppogl/easycppoglConfigVersion.cmake"
    )
endif()

