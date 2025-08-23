# My SDL3 Learning Journey 🚀

This repository contains my personal projects and code snippets as I learn the basics of the Simple DirectMedia Layer 3 (SDL3) library. The goal is to explore 2D graphics, window management, event handling, and more using C++.

## Setup Environment (Windows)

All code in this repository is built and tested on Windows using the MSYS2 and MinGW-w64 environment.

### 1. Install MSYS2
Download and install MSYS2 from the official website: [msys2.org](https://www.msys2.org/)

### 2. Install Required Packages
After installing MSYS2, open the **MSYS2 MINGW64** shell and run the following command to install the C++ compiler, `make`, `git`, and the SDL3 library:

```bash
pacman -S --needed base-devel mingw-w64-x86_64-toolchain git mingw-w64-x86_64-SDL3
```

## Building the Code ⚙️

Each example is in its own folder. Navigate into a project folder and use one of the following methods to compile the code.

### Method 1: Direct Compilation

Compile the C++ source file using `g++` and `pkg-config`. This command automatically finds the correct compiler and linker flags for SDL3.

```bash
g++ main.cpp -o main.exe $(pkg-config --cflags --libs sdl3)
```

### Method 2: Using a Makefile

If a `Makefile` is present in the directory, you can simply run the `make` command to build the project.

```bash
# To compile the project
make

# To clean up the compiled files
make clean
```

## Learning Projects

This is a list of the projects and concepts I am exploring.

* **01_hello_window**: Setting up a basic window and an application loop.
* **02_event_handling**: Managing user input, such as closing the window and keyboard presses.
* **03_drawing_shapes**: Using the SDL renderer to draw basic geometric shapes like rectangles and lines.
* **04_loading_images**: Loading and displaying image files (e.g., `.bmp`, `.png`).
* *(...more to come as I learn!)*

## Useful Resources

* **SDL Wiki:** The official documentation and API reference. [wiki.libsdl.org](https://wiki.libsdl.org/)
* **Lazy Foo' Productions Tutorials:** Excellent, in-depth tutorials for SDL2 that are highly relevant for learning SDL3 concepts. [lazyfoo.net/tutorials/SDL/](https://lazyfoo.net/tutorials/SDL/)
* **SDL GitHub Repository:** The official source code for the SDL library. [github.com/libsdl-org/SDL](https://github.com/libsdl-org/SDL)
* **The Tutorial I Used:** [Constref's Tutorial](https://youtu.be/Wu2g-N5Z78Y?si=CNvzWI7Kd9m2fsxw)