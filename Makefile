CFLAGS 		:= -Wall -Wextra -Werror -pedantic -std=c99 -I include
SRCFILES	:= src/*
OBJFILES	 = $(shell for f in $(SRCFILES); do printf "obj/$$(basename $$f).o "; done)

default: static

objdir:
	mkdir -p obj

static: objdir
	for f in $(SRCFILES); do gcc $(CFLAGS) -c $$f -o "obj/$$(basename $$f).o"; done
	ar rcs cchess.a $(OBJFILES)

dynamic: objdir
	for f in $(SRCFILES); do gcc $(CFLAGS) -fPIC -c $$f -o "obj/$$(basename $$f).o"; done
	gcc -shared $(OBJFILES) -o libcchess.so
