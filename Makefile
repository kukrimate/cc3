CFLAGS = -std=c99 -D_GNU_SOURCE -Wall -g
cc3: cc3.o gen.o sema.o parse.o lex.o util.o
