PROGRAM = rolingBall
CFLAGS = -Wall -Wextra -lGL -lGLU -lglut -lm

$(PROGRAM): ../includes/game.h game.o main.o
	gcc $(CFLAGS) -o $@ game.o main.o
	
game.o: game.c ../includes/game.h
	gcc $(CFLAGS) -c -o $@ $<
	
main.o: main.c ../includes/game.h
	gcc $(CFLAGS) -c -o $@ $<
	
.PHONY : clean

clean: 
	rm *.o $(PROGRAM)
