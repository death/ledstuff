all: evenodd travel

evenodd: evenodd.c led.c
	gcc -Wall -o evenodd evenodd.c led.c ledapp.c -lm -lcurses

travel:	travel.c led.c
	gcc -Wall -o travel travel.c led.c ledapp.c -lm
