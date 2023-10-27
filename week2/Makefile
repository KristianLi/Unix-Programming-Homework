#myfind
myfind:main.o extend.o erro.o
	gcc main.o extend.o erro.o -o myfind
main.o:main.c
	gcc -c main.c
extend.o:extend.c
	gcc -c extend.c
erro.o:erro.c
	gcc -c erro.c
clean:
	rm myfind *.c
