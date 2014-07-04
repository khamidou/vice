##### Makefile #####
# Make file for vice on Linux or compatible OS
# Based on makefile for bait - released to the public domain 2000 by Neil Hodgson neilh@scintilla.org
# This makefile tested with GCC 3.2 and GNOME 2.0

.SUFFIXES: .c .o .h .a

INCLUDEDIRS=-I./scintilla/include
CXXFLAGS= -DGTK -DSCI_LEXER -W -Wall
LEXEROBJS=$(wildcard ../scintilla/gtk/Lex*.o)

all: vice

.c.o:
	gcc `pkg-config --cflags gtk+-2.0` $(INCLUDEDIRS) $(CXXFLAGS) -c $< -o $@
vice: vice.o $(LEXEROBJS) ./scintilla/bin/scintilla.a
	gcc -DGTK $^ -o $@ -lstdc++ `pkg-config --libs gtk+-2.0 gthread-2.0 gmodule-2.0` -lm
clean:
	rm -rf vice *.o
