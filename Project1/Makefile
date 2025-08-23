CXX = g++

EXEC = main.exe

SRCS = main.cpp

SDL_FLAGS = $(shell pkg-config --cflags --libs sdl3)

all: $(EXEC)

$(EXEC): $(SRCS)
	$(CXX) $(SRCS) -o $(EXEC) $(SDL_FLAGS)

clean:
	rm -f $(EXEC)