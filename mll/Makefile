#
# Copyright (C) 2002,2003 Bart Massey and Mick Thomure.
# All Rights Reserved.  See the file COPYING in this directory
# for licensing information.
#

CC = gcc
CFLAGS = -g -Wall
LIBS = -lm
OBJS = bitset.o read_instances.o shuffle_instances.o
OBJS_NBAYES = learn_nbayes.o classify_nbayes.o \
  read_nbayes.o write_nbayes.o kfree_nbayes.o

psam: libmll.a main.o
	$(CC) $(CFLAGS) -o psam main.o libmll.a -lm

libmll.a: $(OBJS_NBAYES) $(OBJS)
	ar cr libmll.a $(OBJS_NBAYES) $(OBJS)

$(OBJS_NBAYES): nbayes.h

bitset.o: bitset.h

bitset.h: bs_bitops.c

read_instances.o: instances.h bitset.h

classify_nbayes.o: classify_nbayes.c
	$(CC) $(CFLAGS) -DUSE_LOGS -c classify_nbayes.c

clean:
	-rm -f libmll.a $(OBJS_NBAYES) $(OBJS) main.o psam

distclean: clean
	-rm -rf autom4te.cache 
	-rm -f configure config.h config.log config.status configure.scan
