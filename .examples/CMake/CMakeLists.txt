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

target_include_directories(${PROJECT_NAME} PRIVATE 
    "../.."
)

# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
 )

find_package( CURL REQUIRED )
target_link_libraries(${PROJECT_NAME} CURL::libcurl )