#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Boost::coroutine" for configuration "Release"
set_property(TARGET Boost::coroutine APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Boost::coroutine PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/boost_coroutine-vc143-mt-x64-1_86.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/boost_coroutine-vc143-mt-x64-1_86.dll"
  )

list(APPEND _cmake_import_check_targets Boost::coroutine )
list(APPEND _cmake_import_check_files_for_Boost::coroutine "${_IMPORT_PREFIX}/lib/boost_coroutine-vc143-mt-x64-1_86.lib" "${_IMPORT_PREFIX}/bin/boost_coroutine-vc143-mt-x64-1_86.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
