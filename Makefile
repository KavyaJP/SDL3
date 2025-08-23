# Define the C++ compiler to use (g++ is the standard for MinGW)
CXX = g++

# Define the name of the final executable program
EXEC = VerifyInstallation.exe

# List all your C++ source files. For now, it's just main.cpp
SRCS = VerifyInstallation.cpp

# Use the 'shell' function to run pkg-config and get all the necessary
# compiler and linker flags for SDL3. This is the automated part.
SDL_FLAGS = $(shell pkg-config --cflags --libs sdl3)

# This is the default rule. When you just type 'make', it will try to
# build the program defined in EXEC.
all: $(EXEC)

# This rule tells 'make' how to create the executable file.
# It depends on your source files. If any source file has changed,
# this rule will run.
$(EXEC): $(SRCS)
	$(CXX) $(SRCS) -o $(EXEC) $(SDL_FLAGS)

# This is a 'phony' rule used for cleaning up. It doesn't create a file.
# It just removes the compiled program.
clean:
	rm -f $(EXEC)