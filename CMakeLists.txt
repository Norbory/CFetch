cmake_minimum_required(VERSION 3.14)
project(gtk_app C)

set(CMAKE_C_STANDARD 11)

# CPU Features
include(FetchContent)
FetchContent_Declare(
  CpuFeatures
  GIT_REPOSITORY https://github.com/google/cpu_features.git
  GIT_TAG        v0.11.0
)
FetchContent_MakeAvailable(CpuFeatures)

# GTK 3.0
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk+-3.0)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
add_executable(prueba prueba.c)

target_link_libraries(prueba PRIVATE cpu_features PkgConfig::GTK)