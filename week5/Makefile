philosopher: philosopher.o error.o lock.o
	gcc -g philosopher.o error.o lock.o -o philosopher -L./lib -lapue -lm

philosopher.o: philosopher.c apue.h lock.h
	gcc -g -c philosopher.c

lock.o:lock.c
	gcc -g -c lock.c

error.o: error.c apue.h
	gcc -g -c error.c

clean:
	rm -f philosopher *.o
