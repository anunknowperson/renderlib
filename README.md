# RenderLib 🚀

![CMake on multiple platforms](https://github.com/anunknowperson/renderlib/actions/workflows/cmake-multi-platform.yml/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
![Codestyle](https://github.com/anunknowperson/renderlib/actions/workflows/сodestyle.yml/badge.svg)

High-performance rendering library using Vulkan API. RenderLib provides a convenient interface for graphics applications, simplifying interactions with low-level graphics.

## 📑 Contents

- [Features](#-features)
- [Requirements](#-requirements)
- [Installation and Building](#-installation-and-building)
- [Contributing](#-contributing)
- [License](#-license)

## ✨ Features

- Abstraction over Vulkan API for convenient use
- Cross-platform support (Windows, Linux)
- Efficient shader management
- Support for various graphic primitives
- Optimized GPU resource handling

## 🔧 Requirements

- **C++23** or newer
- CMake 3.24+
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) 1.4+
- vcpkg for dependency management
- Compiler with C++23 support:
    - MSVC 19.35+ (Visual Studio 2022+)
    - GCC 13+
    - Clang 17+

## 🛠️ Installation and Building

### 1. Clone the Repository

```bash
git clone https://github.com/anunknowperson/renderlib.git
cd renderlib
```

### 2. Install Dependencies

#### Installing Vulkan SDK

Download and install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) according to your operating system.

#### vcpkg Dependencies

The project uses vcpkg for dependency management. You can install it via:
```bash
git submodule update --init
```

### 3. Configure and Build Using CMake Presets

List available configuration presets:

```bash
cmake --list-presets
```

Configure the project with a selected preset:

```bash
cmake --preset wx64-debug  # for Windows Debug
```
```bash
cmake --preset lx64-debug  # for Linux Debug
```

Build the project:

```bash
cmake --build --preset wx64-debug
```
```bash
cmake --build --preset lx64-debug
```

### 4. Running demo

**Note:** Temporary solution - to run the demo you need to copy the `shaders` folder from the project root to the `renderlib\out\build\wx64-debug\demo` directory:

```bash
xcopy /E /I shaders renderlib\out\build\wx64-debug\demo\shaders  # for Windows
```
```bash
cp -r shaders renderlib/out/build/wx64-debug/demo/shaders  # for Linux
```

To launch the demo:
```bash
./out/build/wx64-debug/demo/renderlib.x.exe  # for Windows
```
```bash
./out/build/lx64-debug/demo/renderlib.x  # for Linux
```

## 👥 Contributing

We welcome contributions to the project! If you'd like to contribute:

1. Fork the repository
2. Create a feature branch (`git switch -c amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin amazing-feature`)
5. Open a Pull Request

### Code Style

The project uses clang-format and clang-tidy to maintain a consistent code style. Before submitting a PR, make sure your code matches the formatting standards:

```bash
clang-format-18 -i path/to/your/file.cpp
```

## 📄 License

Distributed under the MIT License. See the `LICENSE` file for more information.