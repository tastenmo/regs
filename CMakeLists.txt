cmake_minimum_required(VERSION 3.18)

#include(cmake/pico_sdk_import.cmake)

project(regs VERSION "0.1.0" LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

option(REGS_OPT_BUILD_EXAMPLES "Build regs examples" ${IS_TOPLEVEL_PROJECT})
option(REGS_OPT_BUILD_TESTS "Build and perform regs tests" ${IS_TOPLEVEL_PROJECT})
option(REGS_OPT_INSTALL "Generate and install regs target" ${IS_TOPLEVEL_PROJECT})

if(REGS_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(REGS_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

include(CMakePackageConfigHelpers)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)

write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT)

if(REGS_OPT_INSTALL)
    include(GNUInstallDirs)

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

    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/package.xml
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
endif()
