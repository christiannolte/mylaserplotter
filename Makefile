all: my_engrave

lex.yy.c: parser.l
	flex parser.l


y.tab.cc: parser.y
	yacc -d parser.y -o y.tab.cc

	
lex.yy.o: lex.yy.c
	cc -c lex.yy.c

my_engrave:  xyplotter.o stepper.o y.tab.cc lex.yy.o my_engrave.cpp
	g++ -o my_engrave  xyplotter.o stepper.o lex.yy.o y.tab.cc my_engrave.cpp


clean:
	rm -f my_engrave
	rm -f *.o
	rm -f y.tab.c
	rm -f y.tab.h
	rm -f y.tab.cc
	rm -f y.tab.hh
	rm -f lex.yy.c
	rm -f lex.yy.cc
	