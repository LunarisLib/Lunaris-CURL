cmake_minimum_required(VERSION 3.15)

set(CMAKE_GENERATOR_PLATFORM x64)
if (WIN32)
    set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
elseif(UNIX AND NOT APPLE)
    set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
else()
    set(VCPKG_TARGET_TRIPLET "x64-osx" CACHE STRING "")
endif()

include ($ENV{CMAKE_TOOLCHAIN_FILE})

project(Testing VERSION 1.0)

# Release?
set(CMAKE_BUILD_TYPE Release)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "_DEBUG")
else ()
    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "NDEBUG")
endif ()


file(GLOB_RECURSE source_list "src/*.cpp")
add_executable(${PROJECT_NAME} ${source_list})

target_include_directories(${PROJECT_NAME} PRIVATE 
    "../.."
)

# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

#if (WIN32) 
#    # Set paths to libcurl installed via Chocolatey
#    set(CURL_LIBRARY $ENV{CURL_LIBRARY_ENV})
#    set(CURL_INCLUDE_DIR $ENV{CURL_INCLUDE_ENV})
#
#    # Message to confirm paths are set (optional for debugging)
#    message(STATUS "CURL_LIBRARY set to: ${CURL_LIBRARY}")
#    message(STATUS "CURL_INCLUDE_DIR set to: ${CURL_INCLUDE_DIR}")
#endif()

find_package( CURL REQUIRED )
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl )