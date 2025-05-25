cmake_minimum_required(VERSION 3.10)

project(banking_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(banking
    banking/Account.cpp
    banking/Transaction.cpp
)

target_include_directories(banking PUBLIC banking)

include(FetchContent)

if(BUILD_TESTS)
    enable_testing()

    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(check tests/tests.cpp)

    target_link_libraries(check PRIVATE banking gmock gmock_main gtest gtest_main pthread)

    # Флаги для корректного сбора покрытия и отключения инлайна
    set(COVERAGE_FLAGS -g -O0 --coverage -fno-inline -fno-inline-functions -fno-inline-small-functions -fno-default-inline -fPIC)

    target_compile_options(banking PRIVATE ${COVERAGE_FLAGS})
    target_link_options(banking PRIVATE --coverage --no-pie)

    target_compile_options(check PRIVATE ${COVERAGE_FLAGS})
    target_link_options(check PRIVATE --coverage --no-pie)

    add_test(NAME check COMMAND check)
endif()
