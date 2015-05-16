CFLAGS=-ansi -pedantic -Wall -Werror 

all:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/main.cpp -o bin/rshell
	g++ $(CFLAGS) src/cp.cpp -o bin/cp
	g++ $(CFLAGS) src/ls.cpp -o bin/ls
	g++ $(CFLAGS) src/mv.cpp -o bin/mv
	g++ $(CFLAGS) src/rm.cpp -o bin/rm
rshell:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/main.cpp -o bin/rshell
ls:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/ls.cpp -o bin/ls
cp:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/cp.cpp -o bin/cp
mv:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/mv.cpp -o bin/mv
rm:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/rm.cpp -o bin/rm
