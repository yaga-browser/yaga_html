CC = g++
IN = main.cpp
OUT = main
CFLAG = -lfltk
CXXFLAGS = -std=c++11 -Wall

all: $(OUT)

$(OUT): $(IN)
	$(CC) $(CXXFLAGS) $(IN) -o $(OUT) $(CFLAG)

.PHONY: all
