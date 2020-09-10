cmake_minimum_required(VERSION 3.10)

# project infomation
project(BuildIndex VERSION 0.1.0
                   DESCRIPTION "build hash index on unordered data file"
                   LANGUAGES CXX)

# place binaries and libraries according to GNU standards
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
# endif()

# generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

# support colored output through Ninja
option(COLORED_OUTPUT "always produce ANSI colored output" ON)
if (${COLORED_OUTPUT})
    if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
        add_compile_options (-fdiagnostics-color=always)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
        add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

# enable asan in debug build
option(ENABLE_ASAN "enable ASAN" OFF)
if (${ENABLE_ASAN})
    if (${CMAKE_BUILD_TYPE} MATCHES Debug OR NOT CMAKE_BUILD_TYPE)
        add_compile_options (-fno-omit-frame-pointer)
        add_compile_options (-fsanitize=address)
        set(LASAN "asan")
        message("enable ASAN")
    else()
        set(LASAN "")
        message("Only in Debug mode could enable ASAN")
    endif()
endif()

# main src
add_subdirectory(src)
add_subdirectory(third_party/MurmurHash)

enable_testing()
add_subdirectory(test)
