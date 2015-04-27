CFLAGS=-ansi -pedantic -Wall -Werror

all:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/main.cpp -o bin/rshell
	g++ $(CFLAGS) src/cp.cpp -o bin/cp

rshell:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/main.cpp -o bin/rshell

cp:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/cp.cpp -o bin/cp
