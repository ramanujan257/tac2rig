out: lex.yy.o parser.tab.o basic_blocks.o line.o
	g++ -std=c++11 -o $@ $^

lex.yy.o: lex.yy.c parser.tab.hpp basic_blocks.hpp
	g++ -std=c++11 -c -o $@ $<

parser.tab.o: parser.tab.cpp parser.tab.hpp basic_blocks.hpp 
	g++ -std=c++11 -c -o $@ $<

parser.tab.cpp parser.tab.hpp: parser.ypp
	bison -d -v $<

lex.yy.c: lexer.lex
	flex $<

line.o: line.cpp line.hpp
	g++ -std=c++11 -c -o $@ $<

basic_blocks.o: basic_blocks.cpp basic_blocks.hpp
	g++ -std=c++11 -c -o $@ $<

.PHONY: clean

clean: 
	rm *.o lex.yy.c *.tab.* out
