CXX = g++

EXEC = main.exe

SRCS = main.cpp

SDL_FLAGS = $(shell pkg-config --cflags --libs sdl3) -mconsole # The -mconsole is added to make it a console application, easier for debugging

all: $(EXEC)

$(EXEC): $(SRCS)
	$(CXX) $(SRCS) -o $(EXEC) $(SDL_FLAGS)

clean:
	rm -f $(EXEC)