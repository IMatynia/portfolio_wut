CC=gcc
ASMBIN=nasm
DEB=gdb

debug: compile
	$(DEB) --args program example_sources/mini_color_test.bmp 3 2

run: compile
	./program example_sources/mini_color_test.bmp 3 2

process_cases: compile
	./program example_sources/black_spot.bmp 200 200
	./program example_sources/blobs.bmp 10 10
	./program example_sources/blobs.bmp 11 11
	./program example_sources/color_dots.bmp 3 3
	./program example_sources/color_dots.bmp 4 20
	./program example_sources/color_dots.bmp 20 4
	./program example_sources/rainbow_corner.bmp 5 16
	./program example_sources/edge.bmp 10 5
	./program example_sources/edge_dots.bmp 7 7
	./program example_sources/rick.bmp 15 15
	./program example_sources/dot.bmp 3 3
	./program example_sources/dot.bmp 1 1
	./program example_sources/mini_color_test.bmp 3 2

compile: asm cc link clean

asm:
	echo "compile asm"
	$(ASMBIN) -o asm_najwieksza_jasnosc.o -f elf64 -g najwieksza_jasnosc.asm

cc :
	echo "compile c++"
	$(CC) -m64 -c -g -O0 *.h *.c

link :
	echo "link"
	$(CC) -m64 -g -o program *.o

clean:
	rm *.o