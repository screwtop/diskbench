PROGRAMS=randread burstread seqread disksize-test sizeof


all: $(PROGRAMS)


randread: randread.o disksize.o
	gcc -o randread randread.o disksize.o -lrt

randread.o: randread.c
	gcc -c -o randread.o randread.c
	
burstread: burstread.c
	gcc -O2 -o burstread burstread.c -lrt

seqread: seqread.o disksize.o
	gcc -o seqread seqread.o disksize.o -lrt
	
seqread.o: seqread.c
	gcc -c -o seqread.o seqread.c

disksize-test: disksize-test.o disksize.o
	gcc -o disksize-test disksize-test.o disksize.o

disksize-test.o: disksize-test.c
	gcc -c -o disksize-test.o disksize-test.c

disksize.o: disksize.c
	gcc -c -o disksize.o disksize.c

sizeof: sizeof.c
	gcc -o sizeof sizeof.c

		
# pl -maxrows 15000 -png -o wd1200jb-random.png access-times-scatter.ploticus
		


.PHONY: clean
clean:
	rm -f $(PROGRAMS) *.o
