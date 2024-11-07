![CMake on multiple platforms](https://github.com/anunknowperson/renderlib/actions/workflows/cmake-multi-platform.yml/badge.svg)

# renderlib

## How to build

### 1. Downloading the repository:

Start by cloning the repository with `git clone --recurse-submodules https://github.com/anunknowperson/renderlib.git`.

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

### 2. Dependencies:

1. Install [Vulkan SDK](https://vulkan.lunarg.com/sdk/home).
2. run the `vcpkg/bootstrap-vcpkg.sh`

### 3. Configure
You can customize the build process using the following CMake options:

-`BUILD_DEMO` (ON by default):  Builds the demo application. Set to OFF to only build the library.

-`BUILD_SHADERS` (ON by default): Builds the shaders. Set to OFF to skip shader compilation.

-`ENABLE_TESTS` (OFF by default): Enables building the unit tests. Set to ON to build the tests.

This project uses CMake and CmakePresets to generate the build files. You can see the list of exsisting presets by running ```cmake --list-presets```. When, after selecting a preset, you can configure it using the following command:
```bash
cmake -B build --preset <preset> -S .
```

### 4. Build

After configuring the project, you can build it using the following command:

```bash
cmake --build build
```
