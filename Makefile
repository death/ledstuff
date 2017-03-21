all: evenodd travel

evenodd: evenodd.c led.c common.c
	gcc -Wall -o evenodd evenodd.c led.c common.c -lm -lcurses

travel:	travel.c led.c common.c
	gcc -Wall -o travel travel.c led.c common.c -lm
