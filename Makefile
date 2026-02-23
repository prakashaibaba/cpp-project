all: main

main: main.cpp
	gcc -g main.cpp -o main -lm

test: main
	./tests/run_tests.sh

clean:
	rm -f main
