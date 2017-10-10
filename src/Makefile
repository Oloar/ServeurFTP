.PHONY: all, clean

# Disable implicit rules
.SUFFIXES:

# Keep intermediate files
#.PRECIOUS: %.o

CC = cc
CFLAGS = -Wall
LDFLAGS =

# Note: -lnsl does not seem to work on Mac OS but will
# probably be necessary on Solaris for linking network-related functions 
# LIBS += -lsocket -lnsl -lrt
LIBS += -lpthread

INCLUDE = csapp.h parse.h ftp.h
OBJS = csapp.o parse.o ftp.o
INCLDIR = -I.

PROGS = clientFTP serverFTP

CLIENTDIR = ./ClientDir/


all: $(PROGS)

%.o: %.c $(INCLUDE)
	$(CC) $(CFLAGS) $(INCLDIR) -c -o $@ $<

%: %.o $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)

clean:
	rm -f $(PROGS) *.o

move:
	mkdir -p $(CLIENTDIR) && mv clientFTP $(CLIENTDIR)/
