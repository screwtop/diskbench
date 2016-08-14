CC=gcc


# libraries:

# NOTE: Mac OS X's uname doesn't support "-o" (any more?).  With no args, the default is to print the kernel name, not the OS name.
OS=$(shell uname)

ifeq ($(OS),Darwin)
	# MAC OS X
	DISKSIZE_LIBS=
	TIMING_LIBS=
else ifeq ($(OS),Linux)
	DISKSIZE_LIBS=-lblkid
	TIMING_LIBS=-lrt
else ifeq ($(OS),NetBSD)
	DISKSIZE_LIBS=
	TIMING_LIBS=-lrt
else
	# everything else (?)
	DISKSIZE_LIBS=
	TIMING_LIBS=-lrt
endif


PROGRAMS=randread burstread seqread rampread disksize-test rewrite sizeof timing_test

DOCS=Readme.txt Readme.html Concepts.txt Concepts.html


all: $(PROGRAMS)


randread: randread.o disksize.o diskbench_timing.o
	$(CC) -o randread randread.o disksize.o diskbench_timing.o $(TIMING_LIBS) $(DISKSIZE_LIBS)

randread.o: randread.c
	$(CC) -c -o randread.o randread.c


rampread: rampread.o disksize.o diskbench_timing.o
	$(CC) -o rampread rampread.o disksize.o diskbench_timing.o $(TIMING_LIBS) $(DISKSIZE_LIBS) -lm

rampread.o: rampread.c
	$(CC) -c -o rampread.o rampread.c

	
burstread: burstread.c diskbench_timing.o disksize.o
	$(CC) -O2 -o burstread burstread.c diskbench_timing.o disksize.o $(TIMING_LIBS) $(DISKSIZE_LIBS)


seqread: seqread.o disksize.o diskbench_timing.o
	$(CC) -o seqread seqread.o disksize.o diskbench_timing.o $(TIMING_LIBS) $(DISKSIZE_LIBS)
	
seqread.o: seqread.c
	$(CC) -c -o seqread.o seqread.c


rewrite: rewrite.o disksize.o
	$(CC) -o rewrite rewrite.o disksize.o $(DISKSIZE_LIBS)

rewrite.o: rewrite.c
	$(CC) -c -o rewrite.o rewrite.c


timing_test: timing_test.c diskbench_timing.o
	$(CC) -o timing_test timing_test.c diskbench_timing.o $(TIMING_LIBS)

disksize-test: disksize-test.o disksize.o
	$(CC) -o disksize-test disksize-test.o disksize.o $(DISKSIZE_LIBS)

disksize-test.o: disksize-test.c
	$(CC) -c -o disksize-test.o disksize-test.c

disksize.o: disksize.c
	$(CC) -c -o disksize.o disksize.c

diskbench_timing.o: diskbench_timing.c diskbench_timing.h
	$(CC) -c -o diskbench_timing.o diskbench_timing.c

sizeof: sizeof.c
	$(CC) -o sizeof sizeof.c

		
# pl -maxrows 15000 -png -o wd1200jb-random.png access-times-scatter.ploticus

# TODO: R
# assuming output from randread > data.txt
#   data<-read.table( "data.txt" )
#   hist( data[[4]], breaks=400, xlim=c(0,35) )
# or something like that


.PHONY: clean
clean:
	rm -f $(PROGRAMS) *.o

doc: $(DOCS)
	xmlto txt Readme.xml

Readme.html: Readme.xml
	xmlto html-nochunks Readme.xml

Concepts.txt: Concepts.xml
	xmlto txt Concepts.xml

Concepts.html: Concepts.xml
	xmlto html-nochunks Concepts.xml
