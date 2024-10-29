# YourProject/tests/CMakeLists.txt


# �׽�Ʈ ���� ����
add_executable(tests
    main.cpp
)



# �׽�Ʈ ��� ���̺귯�� ����
target_link_libraries(tests PRIVATE NetLib)

enable_testing()

find_package(GTest CONFIG REQUIRED)
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(AllTestsInMain tests)

# CTest ���
#enable_testing()
#add_test(NAME YourLibraryTests COMMAND tests)
