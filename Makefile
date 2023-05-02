OUTFILE = light_sampler
OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L  -Werror -Wshadow
light_sampler: main.c  $(OUTDIR)/sampler.o $(OUTDIR)/circlebuffer.o $(OUTDIR)/potdriver.o $(OUTDIR)/terminal.o $(OUTDIR)/lightdips.o $(OUTDIR)/display.o $(OUTDIR)/periodTimer.o $(OUTDIR)/udpserver.o
	$(CC_C) $(CFLAGS)  main.c $(OUTDIR)/sampler.o $(OUTDIR)/circlebuffer.o $(OUTDIR)/potdriver.o $(OUTDIR)/terminal.o  $(OUTDIR)/lightdips.o $(OUTDIR)/display.o $(OUTDIR)/periodTimer.o  $(OUTDIR)/udpserver.o -lpthread -o $(OUTDIR)/$(OUTFILE)

test: display.c 
	$(CC_C) $(CFLAGS) display.c -lpthread -o $(OUTDIR)/test


$(OUTDIR)/udpserver.o: udpserver.c udpserver.h shutdown.h
	$(CC_C) $(CFLAGS) -c udpserver.c -o $(OUTDIR)/udpserver.o

$(OUTDIR)/periodTimer.o: periodTimer.c periodTimer.h
	$(CC_C) $(CFLAGS) -c periodTimer.c -o $(OUTDIR)/periodTimer.o

$(OUTDIR)/display.o: display.c display.h
	$(CC_C) $(CFLAGS) -c display.c -o $(OUTDIR)/display.o

$(OUTDIR)/terminal.o: terminaloutput.c terminaloutput.h
	$(CC_C) $(CFLAGS) -c terminaloutput.c -o $(OUTDIR)/terminal.o

$(OUTDIR)/lightdips.o: lightdips.c lightdips.h
	$(CC_C) $(CFLAGS) -c lightdips.c -o $(OUTDIR)/lightdips.o

$(OUTDIR)/potdriver.o: potDriver.c potDriver.h
	$(CC_C) $(CFLAGS) -c potDriver.c -o $(OUTDIR)/potdriver.o

$(OUTDIR)/sampler.o: sampler.h sampler.c 
	$(CC_C) $(CFLAGS) -c sampler.c -o $(OUTDIR)/sampler.o

$(OUTDIR)/circlebuffer.o: circlebuffer.c circlebuffer.h
	$(CC_C) $(CFLAGS) -c circlebuffer.c -o $(OUTDIR)/circlebuffer.o
clean:
	rm -f $(OUTDIR)/*.o
	rm -f $(OUTDIR)/$(OUTFILE)