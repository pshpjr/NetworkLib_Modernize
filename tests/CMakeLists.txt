# tests/CMakeLists.txt


add_executable(tests
        main.cpp
        LoggerTest.h
)


target_link_libraries(tests PRIVATE NetLib)

enable_testing()

find_package(GTest CONFIG REQUIRED)
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(AllTestsInMain tests)

