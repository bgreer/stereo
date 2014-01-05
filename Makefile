
CC=gcc
FLAGS=

simple_bmp : simple_bmp.c
	${CC} ${FLAGS} simple_bmp.c -o simple_bmp -lm

stereo : main.cpp
	g++ main.cpp -o stereo `pkg-config --libs --cflags opencv`

clean : 
	rm -f stereo simple_bmp
