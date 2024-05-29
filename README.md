![CMake on multiple platforms](https://github.com/anunknowperson/renderlib/actions/workflows/cmake-multi-platform.yml/badge.svg)

# renderlib

## Description

RendererLib is a graphic rendering library written in C++ using the Vulkan API. This library is designed for high-performance graphics applications such as games and visualization tools. RendererLib integrates ImGui for graphical user interfaces and uses Flecs for Entity-Component-System (ECS) management.

## Features

- High-performance rendering with Vulkan API
- Cross-platform support (Linux and Windows)
- Predefined CMake presets for simplified project setup
- Integration with ImGui for GUI development
- Utilizes Flecs for an efficient ECS architecture
- All dependencies managed via vcpkg

## Requirements

- CMake 3.15 or newer
- clang compiler for Linux
- cl compiler for Windows
- Vulkan SDK

## Installation

### Linux

1. Install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home).
2. Clone the repository:

``` 
   git clone --recursive https://github.com/anunknowperson/renderlib.git
   cd rendererlib
```

3. Create a build directory and build the project with vcpkg toolchain:

```
   mkdir build
   cd build
   cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
   make
```

### Windows

1. Install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home).
2. Clone the repository:

```
   git clone --recursive https://github.com/anunknowperson/renderlib.git
   cd rendererlib
```

3. Create a build directory and build the project with vcpkg toolchain:

```
   mkdir build
   cd build
   cmake -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_C_COMPILER="cl" -DCMAKE_CXX_COMPILER="cl" ..
   cmake --build .
```
