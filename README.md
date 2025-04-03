![CMake on multiple platforms](https://github.com/anunknowperson/renderlib/actions/workflows/cmake-multi-platform.yml/badge.svg)

# renderlib

## How to build

### 1. Downloading the repository:

Start by cloning the repository with ```git clone --recurse-submodules https://github.com/anunknowperson/renderlib.git```

If the repository was cloned non-recursively previously, use ```git submodule update --init``` to clone the necessary submodules.

### 2. Dependencies:

1. Install [Vulkan SDK](https://vulkan.lunarg.com/sdk/home).

### 3. Configure

This project uses CMake and CmakePresets to generate the build files. You can see the list of existing presets by running ```cmake --list-presets```. When, after selecting a preset, you can configure it using the following command:

```bash
cmake --preset <preset>
```

### 4. Build

After configuring the project, you can build it using the following command:

```bash
cmake --build --preset <preset>
```
