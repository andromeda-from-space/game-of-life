COMPILER = g++
CFLAGS = -Wall -g
LFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

all: game-of-life

game-of-life: main.cpp sdl-basics.o geneticsolver.o gameoflife.o rng.o
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LFLAGS)

sdl-basics.o: sdl-basics.cpp sdl-basics.h
	$(COMPILER) $(CFLAGS) -c $<

gameoflife.o: gameoflife.cpp gameoflife.h geneticsolver.h rng.h
	$(COMPILER) $(CFLAGS) -c $<

geneticsolver.o: geneticsolver.cpp geneticsolver.h rng.h
	$(COMPILER) $(CFLAGS) -c $<

rng.o: rng.cpp rng.h
	$(COMPILER) $(CFLAGS) -c $<

clean:
	rm -f game-of-life *.o *~ \#*