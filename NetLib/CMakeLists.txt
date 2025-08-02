# NetLib/CMakeLists.txt

# 라이브러리 빌드
cmake_minimum_required(VERSION 3.28)

project(NetLib LANGUAGES CXX)

add_library(NetLib STATIC
        include/Network/HandlerMemory.h
        include/Network/Session.h
        include/Init.h
        include/Network/Session.h
        include/Network/SendBuffer.cpp
        include/Network/SendBuffer.h
        include/Network/IoService.h
        include/third_party/concurrentqueue.h
        include/Logger.h

)

target_sources(NetLib
        PRIVATE
        "src/Allocator.cpp"
        "src/CrashDump.cpp"
        "src/Init.cpp"
        "src/Session.cpp"
        "src/IoService.cpp"
        "src/Logger.cpp"
)
# Specify the required Boost components
find_package(boost_asio CONFIG REQUIRED)
find_package(unofficial-breakpad REQUIRED)
find_package(cpptrace CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)


target_compile_features(NetLib PUBLIC cxx_std_20)
if (WIN32)
    add_custom_command(
            TARGET NetLib POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:cpptrace::cpptrace>
            $<TARGET_FILE_DIR:NetLib>
    )
endif ()


#외부 라이브러리
target_include_directories(NetLib PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(NetLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(NetLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(NetLib PUBLIC Boost::system)
target_link_libraries(NetLib PUBLIC Boost::asio)
target_link_libraries(NetLib PRIVATE unofficial::breakpad::libbreakpad unofficial::breakpad::libbreakpad_client)
target_link_libraries(NetLib PUBLIC cpptrace::cpptrace)
target_link_libraries(NetLib PUBLIC spdlog::spdlog)
