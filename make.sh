#/bin/sh

g++ -o eVehicle eVehicle.cpp Wiegand.cpp OpenCV.cpp `pkg-config opencv --cflags --libs` -lwiringPi -lpthread
