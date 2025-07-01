cmake_minimum_required(VERSION 3.13)
project(llvm-profdata-cross-arch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM package
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set LLVM include and library directories
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Define the source files
set(SOURCES
    ELFSymbolReader.cpp
    llvm-profdata-cross-arch.cpp
)

# Add the executable
add_executable(llvm-profdata-cross-arch ${SOURCES})

# Link against LLVM libraries
llvm_map_components_to_libnames(llvm_libs
    Core
    Support
    Object
    Demangle
)

target_link_libraries(llvm-profdata-cross-arch ${llvm_libs})
