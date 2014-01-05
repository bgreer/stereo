
CC=gcc
FLAGS=


stereo : main.cpp parse_params.c
	g++ main.cpp parse_params.c -o stereo `pkg-config --libs --cflags opencv`

simple_bmp : simple_bmp.c
	${CC} ${FLAGS} simple_bmp.c -o simple_bmp -lm

clean : 
	rm -f stereo simple_bmp
