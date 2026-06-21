# A (Small) Visual Quantum Emulator

A quantum emulator is a piece of software on a traditional computer which emulates the function of a quantum computer. 
This emulator is an implementation of such in C++, which aims to simulate a small number of qubits while providing an understandable user iterface, 
to serve as an educational tool, and to be reasonably optimized.

## Features
(WIP)

## How to Compile
This project is cross-platform should be able to be compiled on Windows, MacOS, and Linux.
However, it has only been verified on Windows 10 and 11.

> [!IMPORTANT]
> After [cloning](https://docs.github.com/en/repositories/creating-and-managing-repositories/cloning-a-repository),
> make sure to initialize the submodules using the command `git submodule update --init --recursive`,
> or add the option `--recurse-submodules` in the initial clone command.
> Otherwise, none of the dependencies will be included and the project will fail to compile.

### Using an IDE
This is the easiest option, but it requires downloading an IDE. I tested this in [CLion](https://www.jetbrains.com/clion/), 
which I personally use and is free for non-commercial use.

#### Pre-requisites
- CLion (or another IDE of your choice)
- The `xkbcommon` development package ([Linux only](https://www.glfw.org/docs/3.4/compile.html#compile_deps))
- The `Wayland` and/or `X11` development packages ([Linux only](https://www.glfw.org/docs/3.4/compile.html#compile_deps))

#### Steps:
- Open the top-level project folder in the IDE.
- The "Open Project Wizard" will prompt you to select which build profiled you would like to use.
Enable `debug-wgpu` and `release-wgpu` (optional), and disable `Debug`, which was enabled by default.
- Click OK, and CMake should set up the project for you (wait for the progress bar in the bottom right).
- In the top right, select your prefered profile (`debug-wgpu` or `release-wgpu`),
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
