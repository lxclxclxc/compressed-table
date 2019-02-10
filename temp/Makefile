# ssdsim linux support
all:ssd 
	
clean:
	rm -f ssd *.o *~
.PHONY: clean

ssd: ssd.o avlTree.o flash.o initialize.o pagemap.o xmatchPro.o bi_list.o dictionary.o diffXmatch.o js_list.o
	cc -g -o ssd ssd.o avlTree.o flash.o initialize.o pagemap.o xmatchPro.o bi_list.o dictionary.o diffXmatch.o js_list.o 
ssd.o: flash.h initialize.h pagemap.h debug.h
	gcc -c -g ssd.c
flash.o: pagemap.h
	gcc -c -g flash.c
initialize.o: avlTree.h pagemap.h
	gcc -c -g initialize.c
pagemap.o: initialize.h
	gcc -c -g pagemap.c
avlTree.o: 
	gcc -c -g avlTree.c
dictionary.o: 
	gcc -c dictionary.c 
diffXmatch.o: 
	gcc -c diffXmatch.c 
js_list.o: 
	gcc -c js_list.c 
xmatchPro.o: 
	gcc -c xmatchPro.c 
#gcc -c -g dictionary.c diffXmatch.c js_list.c xmatchPro.c 
bi_list.o:
	gcc -c  bi_list.c

