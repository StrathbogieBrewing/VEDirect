CC         = gcc

PROGRAM    = vedirect

BUILD      = $(shell date +'%y%m%d%H%M')

CSRC       =  main.c udpReceive.c tty.c ved.c msg.c sig.c fmt.c

BUILDDIR   = bin
SOURCEDIR  = src
EXTDIR     = ext

# include directories
INCLUDE    = -I$(EXTDIR)/AceMessage/src -I$(EXTDIR)/AceBMS
VPATH      = ./$(EXTDIR)/AceMessage/src ./$(SOURCEDIR)

CFLAGS     = -DBUILD="$(BUILD)" -DPROGRAM="$(PROGRAM)" $(INCLUDE) -g

COBJ := $(notdir $(CSRC:.c=.o) )
OBJS := $(patsubst %.o, $(BUILDDIR)/%.o, $(COBJ) )

all: dir $(BUILDDIR)/$(PROGRAM)

dir:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(BUILDDIR)/$(PROGRAM)
	rm $(BUILDDIR)/*.o

# clean files
clean:
	rm -f $(BUILDDIR)/*.o

REMOTEHOST=pi@raspberrypi.local

deploy:
	# upload source code to target
	ssh $(REMOTEHOST) mkdir -p vedirect
	rsync -raL --exclude 'bin' ./ $(REMOTEHOST):vedirect

run:
	./bin/vedirect /dev/ttyUSB1

	# run:
	# 	-pkill tinlog
	# 	nohup ./bin/tinlog /dev/ttyUSB0 </dev/null >/dev/null 2>&1 &
	#
	# TARGET	= pi@raspberrypi:software
	#
	# upload:
	# 	rsync -r ../../ $(TARGET)/Tinduino
	#
