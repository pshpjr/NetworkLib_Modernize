if(OFF)
  include(CMakeFindDependencyMacro)
  find_dependency(ZLIB)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/unofficial-breakpadTargets.cmake")
