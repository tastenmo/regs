include(CheckCXXCompilerFlag)
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)


set(SOURCES test.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(OPTIONS /W4 /WX)
    check_cxx_compiler_flag(/permissive HAS_PERMISSIVE_FLAG)
    if(HAS_PERMISSIVE_FLAG)
        set(OPTIONS ${OPTIONS} /permissive-)
    endif()

    check_cxx_compiler_flag(/std:c++20 HAS_CPP20_FLAG)
    check_cxx_compiler_flag(/std:c++latest HAS_CPPLATEST_FLAG)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(OPTIONS -Wall -Wextra -pedantic-errors -Werror)

    check_cxx_compiler_flag(-std=c++20 HAS_CPP20_FLAG)
endif()



function(make_test src target std)
    add_executable(${target} ${src})
    target_compile_options(${target} PRIVATE ${OPTIONS})
    #target_include_directories(${target} PRIVATE 3rdparty/Catch2/include)
    target_link_libraries(${target} PRIVATE ${CMAKE_PROJECT_NAME} Catch2::Catch2WithMain)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    if(std)
        if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
            target_compile_options(${target} PRIVATE /std:${std})
        else()
            target_compile_options(${target} PRIVATE -std=${std})
        endif()
    endif()
    #add_test(NAME ${target} COMMAND ${target})
    catch_discover_tests(${target} TEST_SUFFIX ${std})
endfunction()

#make_test(test.cpp test-cpp17 c++17)
make_test(testBytes.cpp test_bytes-cpp17 c++17)

if(HAS_CPP20_FLAG)
    make_test(testRegister.cpp test_regs-cpp20 c++20)
    make_test(testBytes.cpp test_bytes-cpp20 c++20)
    make_test(testRegisterPack.cpp testRegisterPack-cpp20 c++20)
    make_test(testBinaryParsing.cpp testBinaryParsing-cpp20 c++20)
endif()

if(HAS_CPPLATEST_FLAG)
    make_test(testRegister.cpp test_regs-cpplatest c++latest)
    make_test(testBytes.cpp test_bytes-cpplatest cpplatest)
    make_test(testRegisterPack.cpp testRegisterPack-cpplatest cpplatest)
    make_test(testBinaryParsing.cpp testBinaryParsing-cpplatest cpplatest)
endif()
