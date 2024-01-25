cmake_minimum_required(VERSION 3.18)

project(regs VERSION "0.1.0" LANGUAGES CXX)
set(PROJECT_DESCRIPTION "Modern C++ library for register manipulation and binary parsing")

#include(cmake/pico_sdk_import.cmake)
include(CheckCXXCompilerFlag)
include(CMakeDependentOption)
include(GNUInstallDirs)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

cmake_dependent_option(REGS_OPT_BUILD_TESTS "Build and perform regs tests" ON
  "BUILD_TESTING;CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)
cmake_dependent_option(REGS_OPT_BUILD_PACKAGE "Build regs Packages" ON
  "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)
option(REGS_OPT_BUILD_EXAMPLES "Build regs examples" ${IS_TOPLEVEL_PROJECT})


set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_VERBOSE_MAKEFILE ON)

    add_compile_options(-stdlib=libc++)

    add_link_options(-stdlib=libc++ -lc++abi)

    check_cxx_compiler_flag(-std=c++20 HAS_CPP20_FLAG)
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT)



install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Config)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION lib/cmake/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}Config
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION .)

export(EXPORT ${PROJECT_NAME}Config
        NAMESPACE ${PROJECT_NAME}::)

#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/package.xml
#        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})





if(REGS_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
    
if(REGS_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
