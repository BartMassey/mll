#
# Copyright (C) 2002 Bart Massey and Mick Thomure.
# All Rights Reserved.  See the file COPYING in this directory
# for licensing information.
#

CC = gcc
CFLAGS = -g -O4 -Wall
LIBS = -lm
OBJS = bitset.o read_instances.o

all: neuron hamming nbayes dtree

neuron: $(OBJS) neuron.o
	$(CC) $(CFLAGS)  -o neuron neuron.o $(OBJS) $(LIBS)

hamming: $(OBJS) hamming.o
	$(CC) $(CFLAGS) -o hamming hamming.o $(OBJS) $(LIBS)

nbayes: $(OBJS) nbayes.o
	$(CC) $(CFLAGS) -DUSE_LOGS -o nbayes nbayes.o $(OBJS) $(LIBS)

dtree: $(OBJS) dtree.o
	$(CC) $(CFLAGS) -o dtree dtree.o $(OBJS) $(LIBS)

neuron.o hamming.o nbayes.o dtree.o: bitset.h read_instances.h

bitset.o: bitset.c bitset.h

bitset.h: bs_bitops.c

read_instances.o: read_instances.c read_instances.h bitset.h

clean:
	-rm -f neuron neuron.o hamming hamming.o \
	  nbayes nbayes.o dtree dtree.o \
	  $(OBJS) *~
