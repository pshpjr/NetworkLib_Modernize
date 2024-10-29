#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "unofficial::breakpad::libbreakpad" for configuration "Debug"
set_property(TARGET unofficial::breakpad::libbreakpad APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(unofficial::breakpad::libbreakpad PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/libbreakpadd.lib"
  )

list(APPEND _cmake_import_check_targets unofficial::breakpad::libbreakpad )
list(APPEND _cmake_import_check_files_for_unofficial::breakpad::libbreakpad "${_IMPORT_PREFIX}/debug/lib/libbreakpadd.lib" )

# Import target "unofficial::breakpad::libbreakpad_client" for configuration "Debug"
set_property(TARGET unofficial::breakpad::libbreakpad_client APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(unofficial::breakpad::libbreakpad_client PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/libbreakpad_clientd.lib"
  )

list(APPEND _cmake_import_check_targets unofficial::breakpad::libbreakpad_client )
list(APPEND _cmake_import_check_files_for_unofficial::breakpad::libbreakpad_client "${_IMPORT_PREFIX}/debug/lib/libbreakpad_clientd.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
