# tests/CMakeLists.txt

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

# Windows에서 CRT 설정 문제 방지
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(tests main.cpp)

target_link_libraries(tests
        PRIVATE
        NetLib
        gtest
        gtest_main
)

target_compile_features(tests PUBLIC cxx_std_20)

enable_testing()

add_test(NAME AllTestsInMain COMMAND tests)
