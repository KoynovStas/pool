cmake_minimum_required(VERSION 3.10)

project(Pool_tests LANGUAGES CXX)

set(CMAKE_CXX_FLAGS_DEBUG   "-DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")
set(CMAKE_CXX_FLAGS         "-O2 -g  -DSTEST_ONLY_BASENAME")
if(UNIX)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DSTEST_UNIX_SLASH")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()


# C++17 support for new of over-aligned types in GCC(since 7.x)
# can be enabled with the -faligned-new flag.
# This flag is enabled by default for -std=c++17.
# But gcc(mingw) version 7.х require this flag to be enabled manually.
# For other compilers, check its documentation.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
        add_compile_options(-faligned-new=8 -Waligned-new)
    endif()
endif()


message("-- Generator is set to: ${CMAKE_GENERATOR}")



set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../src)


set(SOURCES main.cpp
            test_spool_list.cpp
            test_spool_list_bitset.cpp
            test_spool_dlist.cpp
            test_pool_list.cpp
            test_pool_list_block.cpp
            test_pool_dlist.cpp
            test_pool_dlist_block.cpp
            )

set(HEADERS stest.h
            helpers.h
            base_tests.h
            ex_tests.h
            ex_dynamic_tests.h
            iterator_tests.h
            ${INCLUDE_DIR}/pool.h
            )

add_executable(tests ${SOURCES} ${HEADERS})

add_custom_target(run_tests ALL COMMAND tests DEPENDS tests)

target_include_directories(tests PRIVATE ${INCLUDE_DIR})
