main:
	g++ -g src/*.cpp src/*.h* src/discs/* main.cpp -o bin/main

testing:
	g++ -g src/*.cpp src/*.h* src/discs/* catch.hpp catch_config.cpp tests/* -o bin/testing

docs_gen:
	doxygen

open_doc:
	xdg-open doc/html/index.html
