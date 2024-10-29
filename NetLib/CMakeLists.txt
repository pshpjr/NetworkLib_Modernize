# YourProject/NetLib/CMakeLists.txt

# 라이브러리 빌드
cmake_minimum_required (VERSION 3.28)

project(NetLib LANGUAGES CXX)

add_library(NetLib STATIC)

target_sources(NetLib
   PRIVATE
   "src/Allocator.cpp"
   "src/Utils.cpp"
   "src/CrashDump.cpp"
)

target_include_directories(NetLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(NetLib PUBLIC cxx_std_20)

#외부 라이브러리
target_link_libraries(NetLib PRIVATE Boost::system)
  target_link_libraries(NetLib PRIVATE unofficial::breakpad::libbreakpad unofficial::breakpad::libbreakpad_client)
