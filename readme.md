# My SDL3 Learning Journey 🚀

This repository contains my personal projects and code snippets as I learn the basics of the Simple DirectMedia Layer 3 (SDL3) library. The goal is to explore 2D graphics, window management, event handling, and more using C++.

## Setup Environment (Windows)

All code in this repository is built and tested on Windows using the MSYS2 and MinGW-w64 environment.

### 1. Install MSYS2
Download and install MSYS2 from the official website: [msys2.org](https://www.msys2.org/)

### 2. Install Required Packages
After installing MSYS2, open the **MSYS2 MINGW64** shell and run the following command to install the C++ compiler, `make`, `git`, and the SDL3 library:

Upgrade the packages

```bash
pacman -Syu # After this the MSYS2 shell might close so open it again and run the next command
pacman -Su
```

Download the `g++` compiler

```bash
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
```

Then open the MINGW64 Shell and run this command to install sdl3

```bash
pacman -S mingw-w64-x86_64-sdl3 mingw-w64-x86_64-sdl3-image mingw-w64-x86_64-sdl3-ttf mingw-w64-x86_64-make
```

And now to verify the installation of all these libraries, use this command in the root directory of this repository after cloning it

### 3. Clone The repository

```bash
git clone https://github.com/KavyaJP/SDL3.git
cd SDL3
```

### 4. Verify Installation

```bash
make
```

this should create a VerifyInstallation.exe and after running the exe, you should see a black screen window popup for 3 seconds

## Building the Code ⚙️

Each example is in its own folder. Navigate into a project folder and use one of the following methods to compile the code.


### Build Method 1: Direct Compilation

Compile the C++ source file using `g++` and `pkg-config`. This command automatically finds the correct compiler and linker flags for SDL3.

```bash
g++ main.cpp -o main.exe $(pkg-config --cflags --libs sdl3)
```

### Build Method 2: Using a Makefile

If a `Makefile` is present in the directory, you can simply run the `make` command to build the project.

```bash
# To compile the project
make

# To clean up the compiled files
make clean
```

## Useful Resources

* **SDL Wiki:** The official documentation and API reference. [wiki.libsdl.org](https://wiki.libsdl.org/)
* **Lazy Foo' Productions Tutorials:** Excellent, in-depth tutorials for SDL2 that are highly relevant for learning SDL3 concepts. [lazyfoo.net/tutorials/SDL/](https://lazyfoo.net/tutorials/SDL/)
* **SDL GitHub Repository:** The official source code for the SDL library. [github.com/libsdl-org/SDL](https://github.com/libsdl-org/SDL)
* **The Tutorial I Used:** [Constref's Tutorial](https://youtu.be/Wu2g-N5Z78Y?si=CNvzWI7Kd9m2fsxw)