out: test_ideje.o basic_blocks.o line.o
	g++ -std=c++11 -o $@ $^

test_ideje.o: test_ideje.cpp line.hpp basic_blocks.hpp
	g++ -std=c++11 -c -o $@ $<

line.o: line.cpp line.hpp
	g++ -std=c++11 -c -o $@ $<

basic_blocks.o: basic_blocks.cpp basic_blocks.hpp
	g++ -std=c++11 -c -o $@ $<

.PHONY: clean

clean: 
	rm *.tab.* *.o lex.yy.c out
