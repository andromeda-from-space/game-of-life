COMPILER = g++
CFLAGS = -Wall
LFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
CFLAGS_DEBUG = -Wall -g

all: game-of-life

game-of-life: main.cpp sdl-basics.o geneticsolver.o gameoflife.o rng.o
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LFLAGS)

sdl-basics.o: sdl-basics.cpp sdl-basics.h
	$(COMPILER) $(CFLAGS) -c $<

gameoflife.o: gameoflife.cpp gameoflife.h geneticsolver.h rng.h
	$(COMPILER) $(CFLAGS) -c $<

geneticsolver.o: geneticsolver.cpp geneticsolver.h rng.h
	$(COMPILER) $(CFLAGS) -c $<

cellularautomata.o: cellularautomata.cpp cellularautomata.h rng.h
	$(COMPILER) $(CFLAGS) -c $<

rng.o: rng.cpp rng.h
	$(COMPILER) $(CFLAGS) -c $<

debug: debug.cpp sdl-basics-debug.o
	$(COMPILER) $(CFLAGS_DEBUG) -o $@ $^ $(LFLAGS)

sdl-basics-debug.o: sdl-basics.cpp sdl-basics.h
	$(COMPILER) $(CFLAGS_DEBUG) -c $< -o sdl-basics-debug.o

clean:
	rm -f game-of-life debug *.o *~ \#*