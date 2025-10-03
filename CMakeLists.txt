cmake_minimum_required(VERSION 3.22)
project(SimpleConfig
  VERSION 1.0.0
  DESCRIPTION "SimpleConfig module"
  LANGUAGES C)

# General configuration / variables
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
enable_testing()

# Vendor dependencies
add_subdirectory(vendor)

# Find packages
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

# Modules
add_subdirectory(scf)
