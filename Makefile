# Compiles .c and .cc programs.
CC=gcc
CPP=g++
# Pick your own favorite headers and libraries.
#INCDIRS = -I/usr/include/GL
#LDLIBS = -lGL -lGLEW -lglut -lGLU
LDLIBS = -lpng
#CFLAGS = $(shell pkg-config --cflags opencv)
#LIBS = $(shell pkg-config --libs opencv)
# Include this for debugging symbols.
#FLAGS = -ggdb -Wall
FLAGS = -Wall -std=c99

ALLCSRCS := $(wildcard *.c)
ALLCPPSRCS := $(wildcard *.cc)
ALLEXEC := $(ALLCSRCS:.c=) $(ALLCPPSRCS:.cc=)
all: $(ALLEXEC)
%: %.c
	$(CC) $(FLAGS) $(INCDIRS) -o $@ $< $(LDLIBS)
%: %.cc
	$(CPP) $(FLAGS) $(INCDIRS) -o $@ $< $(LDLIBS)

clean:
	rm -f *.o $(ALLEXEC)
.PHONY: all clean
