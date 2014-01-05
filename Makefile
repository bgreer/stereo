


stereo : main.cpp ocv.cpp parse_params.c
	g++ main.cpp ocv.cpp parse_params.c -o stereo `pkg-config --libs --cflags opencv`


clean : 
	rm -f stereo
