# YourProject/CMakeLists.txt

cmake_minimum_required (VERSION 3.28)
project(StaticLib)

# Specify the required Boost components
find_package(Boost REQUIRED COMPONENTS system)
find_package(unofficial-breakpad REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

if(Boost_FOUND)
    # Include Boost headers
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found")
endif()

# 서브디렉터리 추가
add_subdirectory(NetLib)
add_subdirectory(tests)

