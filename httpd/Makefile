#
# Makefile for http stuff
#

#CFLAGS += -DDEBUG_HTTP -DUIP_LOGGING=1
#CFLAGS += -DDEBUG_HTTP 

include $(TOPDIR)/config.mk

LIB   = libhttpd.a

OBJS += uip.o uip_arch.o uip_arp.o httpd.o fs.o cgi.o hwid_def.o

all:  $(LIB)

$(LIB):  $(START) $(OBJS)
	   $(AR) crv $@ $(OBJS)


#%.o:
#	   $(CC) $(CFLAGS) -c $(<:.o=.c)

#########################################################################

.depend: Makefile $(OBJS:.o=.c)
	      $(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################

clean:
	rm -f *.o *.a