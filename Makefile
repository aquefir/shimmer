all:
	clang -O3 tests/main.c src/*.c -o test
	clang -O3 tests/view.c src/*.c -o view
