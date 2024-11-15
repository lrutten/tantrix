all: build

rebuild: src/tantrix.cpp
	cd build;make

build: src/tantrix.cpp
	mkdir build;cd build;cmake ..;make

valgrind: build/tantrix
	valgrind --leak-check=full --show-leak-kinds=all build/tantrix

install: build/tantrix
	cp -v build/tantrix ~/bin

docs:
	mkdir docs
	doxygen doxy-config

clean:
	rm -Rvf build

