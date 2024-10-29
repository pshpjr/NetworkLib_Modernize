# YourProject/tests/CMakeLists.txt


# 테스트 실행 파일
add_executable(tests
    main.cpp
)



# 테스트 대상에 라이브러리 연결
target_link_libraries(tests PRIVATE NetLib)

# CTest 등록
#enable_testing()
#add_test(NAME YourLibraryTests COMMAND tests)
