myfind: myfind.o error.o
	gcc -g myfind.o error.o -o myfind -L./lib -lapue -lm

myfind.o: myfind.c apue.h
	gcc -g -c myfind.c

error.o: error.c apue.h
	gcc -g -c error.c

clean:
	rm -f myfind *.o
