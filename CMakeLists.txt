cmake_minimum_required(VERSION 3.17)
project(VideoFilterApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For better vcpkg integration
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

set(OpenCV_ROOT "C:/opencv-mingw/x64/mingw/lib/cmake/opencv4")

# Find required packages
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

# Print OpenCV information for debugging
message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBRARIES: ${OpenCV_LIBRARIES}")
message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

# Add source files
file(GLOB_RECURSE SOURCES
        "src/*.cpp"
        "src/*.h"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries - using more explicit linking
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCV_LIBS}
        Threads::Threads
)

# Add compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
        _USE_MATH_DEFINES
        NOMINMAX
)

# Installation rules (optional)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)