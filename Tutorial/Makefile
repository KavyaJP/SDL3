# Define the C++ compiler to use
CXX = g++

# Define the name of the final executable program
EXEC = main.exe

# List all your C++ source files
SRCS = main.cpp

# Use c++20 compiler
CXX_FLAGS = -std=c++20

# Use the correct library name 'SDL3_image' for pkg-config
SDL_FLAGS = $(shell pkg-config --cflags --libs sdl3 sdl3-image) -mconsole

# Default rule: build the executable
all: $(EXEC)

# Rule to create the executable
$(EXEC): $(SRCS)
	$(CXX) $(SRCS) -o $(EXEC) $(CXX_FLAGS) $(SDL_FLAGS)

# Rule to clean up the build files
clean:
	rm -f $(EXEC)