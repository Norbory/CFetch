cmake_minimum_required(VERSION 3.14)
project(cfetch C)

set(CMAKE_C_STANDARD 11)

# Descargar e integrar cpu_features automáticamente desde GitHub
include(FetchContent)
FetchContent_Declare(
  CpuFeatures
  GIT_REPOSITORY https://github.com/google/cpu_features.git
  GIT_TAG        v0.11.0
)
FetchContent_MakeAvailable(CpuFeatures)

# Tu ejecutable
add_executable(cfetch cfetch.c)

# Enlazar la librería a tu ejecutable
target_link_libraries(cfetch PRIVATE cpu_features)