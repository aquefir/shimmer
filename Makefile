all:
	clang -O3    tests/main.c src/*.c -o test
	clang -O3    tests/view.c src/*.c -o view
	
debug:
	clang -Og -O0 -g tests/main.c src/*.c -o test
	clang -Og -O0 -g tests/view.c src/*.c -o view
