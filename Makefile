PROGRAMS=randread burstread seqread rampread disksize-test rewrite sizeof timing_test

DOCS=Readme.txt Readme.html Concepts.txt Concepts.html


all: $(PROGRAMS)


randread: randread.o disksize.o diskbench_timing.o
	gcc -o randread randread.o disksize.o diskbench_timing.o -lrt

randread.o: randread.c
	gcc -c -o randread.o randread.c


rampread: rampread.o disksize.o diskbench_timing.o
	gcc -o rampread rampread.o disksize.o diskbench_timing.o -lrt -lm

rampread.o: rampread.c
	gcc -c -o rampread.o rampread.c

	
burstread: burstread.c diskbench_timing.o
	gcc -O2 -o burstread burstread.c diskbench_timing.o -lrt


seqread: seqread.o disksize.o diskbench_timing.o
	gcc -o seqread seqread.o disksize.o diskbench_timing.o -lrt
	
seqread.o: seqread.c
	gcc -c -o seqread.o seqread.c


rewrite: rewrite.o disksize.o
	gcc -o rewrite rewrite.o disksize.o

rewrite.o: rewrite.c
	gcc -c -o rewrite.o rewrite.c


timing_test: timing_test.c diskbench_timing.o
	gcc -o timing_test timing_test.c diskbench_timing.o -lrt

disksize-test: disksize-test.o disksize.o
	gcc -o disksize-test disksize-test.o disksize.o

disksize-test.o: disksize-test.c
	gcc -c -o disksize-test.o disksize-test.c

disksize.o: disksize.c
	gcc -c -o disksize.o disksize.c

diskbench_timing.o: diskbench_timing.c diskbench_timing.h
	gcc -c -o diskbench_timing.o diskbench_timing.c

sizeof: sizeof.c
	gcc -o sizeof sizeof.c

		
# pl -maxrows 15000 -png -o wd1200jb-random.png access-times-scatter.ploticus
		


.PHONY: clean
clean:
	rm -f $(PROGRAMS) *.o

doc: $(DOCS)

Readme.txt: Readme.xml
	xmlto txt Readme.xml

Readme.html: Readme.xml
	xmlto html-nochunks Readme.xml

Concepts.txt: Concepts.xml
	xmlto txt Concepts.xml

Concepts.html: Concepts.xml
	xmlto html-nochunks Concepts.xml
