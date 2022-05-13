CFLAGS = -std=c99 -D_GNU_SOURCE -Wall -Wimplicit-fallthrough -g
cc3: cc3.o gen.o sema.o parse.o lex.o debug.o util.o
clean:
	rm -f *.o cc3
