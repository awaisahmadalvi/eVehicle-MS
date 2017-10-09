
CC=g++

CFLAGS=`pkg-config opencv --cflags --libs` -lwiringPi -lpthread -Isrc/  

SRC_FILES=src/eVehicle.cpp src/Wiegand.cpp src/OpenCV.cpp

BIN_FILE=eVehicle

all: clean build

build:
	@$(CC) -o bin/$(BIN_FILE)  $(SRC_FILES) $(CFLAGS)
	@echo '*** Build Completed ***'

run:
	@echo '*** Server Running ***'
	@bin/./$(BIN_FILE)

clean:
	@rm -rf bin/*.o src/*~
	@echo '*** Clean Completed ***'
