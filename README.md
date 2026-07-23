# A (Small) Visual Quantum Emulator

A quantum emulator is a piece of software which emulates the function of a quantum computer, using a normal (traditional) computer.

This specific emulator is written in C++, and aims to simulate a small number of qubits while providing an understandable user interface, 
to serve as an educational tool, and to be reasonably optimized.

## Table of Contents
- [Features](#features)
- [How to compile](#how-to-compile)
  - [How to compile using an IDE](#using-an-ide)
  - [How to compile using the commandline](#using-the-commandline)
  - [How to compile for the web](#for-the-web)
- [Running Tests](#running-tests)
- [Contributing](#contributing)
- [License](#license)

## Features
A small-scale quantum emulator. Can simulate a small number of qubits (~8), and display information about the circuit.

## How to Compile
This project is theoretically cross-platform should be able to be compiled on Windows, macOS, and Linux.
However, I've only verified this on Windows 10 and 11.

> [!IMPORTANT]
> After [cloning](https://docs.github.com/en/repositories/creating-and-managing-repositories/cloning-a-repository),
> make sure to initialize the submodules using the command `git submodule update --init --recursive`,
> or add the option `--recurse-submodules` in the initial clone command.
> Otherwise, none of the dependencies will be included and the project will fail to compile.

### Using an IDE
This is the easiest option, but it requires downloading an IDE. I tested this in [CLion](https://www.jetbrains.com/clion/), 
which I personally use and is free for non-commercial use.

#### Pre-requisites:
- CLion (or another IDE of your choice)
- The `xkbcommon` development package ([Linux only](https://www.glfw.org/docs/3.4/compile.html#compile_deps))
- The `Wayland` and/or `X11` development packages ([Linux only](https://www.glfw.org/docs/3.4/compile.html#compile_deps))

#### Steps:
- Open the top-level project folder in the IDE.
- The "Open Project Wizard" will prompt you to select which build profiled you would like to use.
Enable `debug-wgpu`, `release-wgpu` (optional), and `gtest-wgpu` (if you want to [run tests](#running-tests));
and disable `Debug`, which was enabled by default.
- Click OK, and CMake should set up the project for you (wait for the progress bar in the bottom right).
- In the top right, select your preferred profile (`debug-wgpu` or `release-wgpu`),
ensure that "**App**" is selected as the configuration, and click the green play button.
- The project should be built and run!

### Using the Commandline
The C++ build system is infamously atrocious, which is why I recommend you use an IDE,
but building using the terminal shouldn't be too difficult if you've used it before.

#### Pre-requisites:
- [CMake](https://cmake.org/download/)
- [Ninja](https://ninja-build.org/)
- [LLVM/ Clang](https://github.com/mstorsjo/llvm-mingw)
- (Windows only) all the above programs added to your system's PATH variables (i.e., `C:\<path-to-llvm>\bin`, `C:\<path-to-ninja>`, and `C:\<path-to-cmake>\bin`).
- The `xkbcommon` development package ([Linux only](https://www.glfw.org/docs/3.4/compile.html#compile_deps))
- The `Wayland` and/or `X11` development packages ([Linux only](https://www.glfw.org/docs/3.4/compile.html#compile_deps))

#### Steps:
- Open a terminal in the project directory.
- Run `cmake . --preset <preset-name>`.
(Use `cmake --list-presets` for a list of all available presets. If you're unsure, use `debug-wgpu`.)
- Run `cmake --build build/<preset-name>`
- If everything went well, the executable should be found at `build\<preset-name>\dist\<platform>\App.exe`, along with `wgpu_native.dll`.
- Running `App.exe` will run the program. If you wish to distribute this executable remember to also include everything else in the folder. 

> [!NOTE]
> If you get an error message like: `No CMAKE_CXX_COMPILER could be found` or similar,
> you probably didn't install all the prerequisites, or you didn't properly add them to your System PATH.

> [!NOTE]
> On Linux: if you don't want to install both Wayland and X11,
> you can build for only one using the [respective CMake options](https://www.glfw.org/docs/3.4/compile_guide.html#compile_options_unix).

### For the Web
This project can also be run on the web! It's a bit finicky, so be warned.

#### Pre-requisites
- [CMake](https://cmake.org/download/)
- [Emscripten](https://emscripten.org/docs/getting_started/downloads.html) (on Windows, in your PATH)
- [Python 3.8](https://www.python.org/downloads/) or above.
- [LLVM/ Clang](https://github.com/mstorsjo/llvm-mingw)
- Xcode Command Line Tools (macOS Only)
- 10.14 Mojave or above (macOS Only)

#### Steps:
In the root project folder, run `web-build.sh` (or `.bat` if you're on Windows);
or, run the following commands in your terminal:
```bash
# Generate the cmake files. We're building from source, 
# since I couldn't get the pre-compiled option to work.
emcmake cmake -B build\web -DWEBGPU_BACKEND=EMDAWNWEBGPU -DWEBGPU_BUILD_FROM_SOURCE=ON

# Clean the build directory and then build the project.
# (The --clean-first flag is optional.)
cmake --build build\web --clean-first

# Run the app
# Will be available at http://localhost:8000/App.html
python -m http.server -d build\web\dist\Emscripten
```

## Running Tests
This project uses GoogleTest (gtest) for running C++ tests.

### Tests in an IDE
Running the tests in an IDE is straightforward, most have build-in support. 
For example, in CLion, simply select `EngineTest` from the configurations dropdown in the top right.

> [!TIP]
> Make sure that the CMake profile is enabled in CLion!

### Tests from the Commandline
See [compiling using the commandline](#using-the-commandline). 
The process is the same as compiling normally, you just have to choose the `gtest-wgpu` profile.

After building, if you try to open `EngineTest.exe` from the file explorer, it will open and close immediately. 
To keep it open (so you can actually see the results), run the program from the commandline:
```cmd
.\build\gtest-wgpu\dist\<platform>\EngineTest.exe
```

## Contributing
All contributions are welcome! If you make a PR, try to follow the code style:
- Follow the included `.clang-format` style! 
- You should follow the convention of using the `.cc` and `.h` file extensions for C++ source and header files respectively.
- Contrary to some textbook conventions, matrices and the circuit diagrams are zero-indexed!

## License
This project is licensed under the Unlicense ([unlicense.org](https://unlicense.org/)).

This project uses the following third-party libraries:
- [Dear ImGUI](https://github.com/ocornut/imgui) ([MIT License](https://github.com/ocornut/imgui/blob/master/LICENSE.txt))
- [ImPlot](https://github.com/epezent/implot) ([MIT License](https://github.com/epezent/implot/blob/master/LICENSE))
- [GLFW](https://github.com/glfw/glfw) ([Zlib License](https://github.com/glfw/glfw/blob/master/LICENSE.md))
- [GLFW WebGPU Extension](https://github.com/eliemichel/glfw3webgpu) ([MIT License](https://github.com/eliemichel/glfw3webgpu/blob/main/LICENSE.txt))
- [Google Test](https://github.com/google/googletest) ([BSD 3-Clause "New" or "Revised" License](https://github.com/google/googletest/blob/main/LICENSE))
- [WebGPU Distribution](https://github.com/eliemichel/WebGPU-distribution) ([MIT License](https://github.com/eliemichel/WebGPU-distribution/blob/main/LICENSE.txt))


The WebGPU distribution can be configured to use the following third-party backends (though only one is used at once):
- [Dawn](https://dawn.googlesource.com/dawn) ([Apache 2.0 License](https://dawn.googlesource.com/dawn/+/HEAD/LICENSE))
- [Emscripten](https://github.com/emscripten-core/emscripten) (Dual [MIT/University of Illinois/NCSA Open Source](https://github.com/emscripten-core/emscripten/blob/main/LICENSE) License)
- [wgpu-native](https://github.com/gfx-rs/wgpu-native) (Dual [MIT](https://github.com/gfx-rs/wgpu-native/blob/trunk/LICENSE.MIT)/[Apache 2.0](https://github.com/gfx-rs/wgpu-native/blob/trunk/LICENSE.APACHE) License)
