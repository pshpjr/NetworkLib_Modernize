# NetLib/CMakeLists.txt

# 라이브러리 빌드
cmake_minimum_required(VERSION 3.28)

project(NetLib LANGUAGES CXX)

add_library(NetLib STATIC
        include/Network/DefaultHandlerAllocator.h
        include/Network/Session.h
        include/Init.h

)

target_sources(NetLib
        PRIVATE
        "src/Allocator.cpp"
        "src/CrashDump.cpp"
        "src/Init.cpp"
)
# Specify the required Boost components
find_package(Boost REQUIRED COMPONENTS asio)
find_package(Boost REQUIRED COMPONENTS system)
find_package(unofficial-breakpad REQUIRED)
find_package(cpptrace CONFIG REQUIRED)
find_package(Flatbuffers CONFIG REQUIRED)


if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found")
endif ()

target_compile_features(NetLib PUBLIC cxx_std_20)
if (WIN32)
    add_custom_command(
            TARGET NetLib POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:cpptrace::cpptrace>
            $<TARGET_FILE_DIR:NetLib>
    )
endif ()

#####loguru lib setting

include(FetchContent)
FetchContent_Declare(LoguruGitRepo
        GIT_REPOSITORY "https://github.com/emilk/loguru" # can be a filesystem path
        GIT_TAG "master"
)
# set any loguru compile-time flags before calling MakeAvailable()
set(LOGURU_WITH_STREAMS TRUE)
FetchContent_MakeAvailable(LoguruGitRepo) # defines target 'loguru::loguru'
#####

#외부 라이브러리
target_include_directories(NetLib PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(NetLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(NetLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(NetLib PUBLIC loguru::loguru)
target_link_libraries(NetLib PUBLIC Boost::system)
target_link_libraries(NetLib PUBLIC Boost::asio)
target_link_libraries(NetLib PUBLIC unofficial::breakpad::libbreakpad unofficial::breakpad::libbreakpad_client)
target_link_libraries(NetLib PRIVATE cpptrace::cpptrace)
