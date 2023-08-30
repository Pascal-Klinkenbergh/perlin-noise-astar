path: main.cc include/*.hpp
	clang++ -Wall -std=c++20 -Iinclude `pkg-config --libs sfml-graphics` main.cc -o path

run: path
	./path

.PHONY: clean
clean: 
	rm path