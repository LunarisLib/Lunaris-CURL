cmake_minimum_required(VERSION 3.15)

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

# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
 )
 
 
# Try to find the system-installed CURL without the REQUIRED flag to avoid failure
find_package(CURL)

if(NOT CURL_FOUND)
    message(STATUS "libcurl not found, downloading and building it...")

    # Use ExternalProject to download and build libcurl
    include(ExternalProject)

    # Set the URL for the curl GitHub repository
    set(CURL_REPO_URL "https://github.com/curl/curl.git")
    
    # Get the latest git tag (latest release version) dynamically
    execute_process(
        COMMAND git ls-remote --tags ${CURL_REPO_URL}
        COMMAND sed -n 's/.*refs\/tags\/\([0-9]*\.[0-9]*\.[0-9]*\)$/\1/p'  # Filter tag names
        COMMAND sort -V  # Sort versions in ascending order
        COMMAND tail -n 1  # Get the latest tag (last tag in sorted order)
        OUTPUT_VARIABLE CURL_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Print the latest version to verify it's correct
    message(STATUS "Latest CURL version is ${CURL_VERSION}")

    set(CURL_SRC_DIR "${CMAKE_BINARY_DIR}/curl_source")
    set(CURL_BUILD_DIR "${CMAKE_BINARY_DIR}/curl_build")
    set(CURL_INSTALL_DIR "${CMAKE_BINARY_DIR}/curl_install")

    # Define the ExternalProject to download, build, and install libcurl
    ExternalProject_Add(
        libcurl
        PREFIX ${CURL_SRC_DIR}
        GIT_REPOSITORY ${CURL_REPO_URL}
        GIT_TAG ${CURL_VERSION}  # Use the latest version fetched above
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CURL_INSTALL_DIR}
                   -DCMAKE_BUILD_TYPE=Release
    )

    # Add dependency to ensure that libcurl is built before continuing
    add_dependencies(${PROJECT_NAME} libcurl)

    # Set the appropriate variables to use libcurl in your project
    set(CURL_INCLUDE_DIR ${CURL_INSTALL_DIR}/include)
    set(CURL_LIBRARY ${CURL_INSTALL_DIR}/lib/libcurl.a)  # Static library (adjust as necessary)
else()
    message(STATUS "libcurl found on system")
    # If CURL was found, no need to build it from source
    set(CURL_INCLUDE_DIR ${CURL_INCLUDE_DIRS})
    set(CURL_LIBRARY ${CURL_LIBRARIES})
endif()


target_include_directories(${PROJECT_NAME} PRIVATE 
    "../.."
    ${CURL_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARY})