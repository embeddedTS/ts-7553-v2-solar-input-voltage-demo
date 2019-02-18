PROGNAME = solar
INSTALL = /usr/local/bin

PWD		= $(shell pwd)

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc


OBJS := solar.o lcd-display.o tsmicroctl.o

INCLUDES = -I/usr/include -I/usr/include/cairo
LIBS = -lcairo
CFLAGS = $(INCLUDES) -Wall -O3
LDFLAGS = $(LIBS)
	
$(PROGNAME):  $(OBJS)	
	$(CC) $(OBJS) $(LDFLAGS) -o $@

%.obj : %.c 
	@$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

install: 
	install -m 4755 $(PROGNAME) $(INSTALL)
   
clean:
	rm -f $(OBJS) $(PROGNAME)
	rm -f $(INSTALL)/$(PROGNAME)

