#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "easycppogl" for configuration "Release"
set_property(TARGET easycppogl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(easycppogl PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/easycppogl.lib"
  )

list(APPEND _cmake_import_check_targets easycppogl )
list(APPEND _cmake_import_check_files_for_easycppogl "${_IMPORT_PREFIX}/lib/easycppogl.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
