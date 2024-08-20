
COMPILER_ARGS = -Wall -Wextra -O -g

INCLUDE = -Iinclude/types.h
SOURCE = include/types.cpp

quada: src/main.cpp lib/libstd.so
	@mkdir -p bin
	clang++ -Iinclude $(COMPILER_ARGS) -Llib -lstd -Wl,-rpath,lib -o quada src/main.cpp

bin/parser.o: include/parser.h include/parser.cpp
	@mkdir -p bin
	clang -c -o $@ -I$^ $(INCLUDE) -fPIC -g

bin/standard.o: include/standard.h include/standard.cpp
	@mkdir -p bin
	clang -c -o $@ -I$^ -fPIC -g

lib/libstd.so: bin/parser.o bin/standard.o
	@mkdir -p lib
	clang -shared -o $@ $^ $(INCLUDE) $(SOURCE) -g

install: quada
	sudo cp $< /usr/local/bin

remove:
	sudo rm /usr/local/bin/quada

all: quada

clean:
	rm -f bin/*.o
	rm -f bin/*.obj
	rm -f quada
	rm -f lib/*.so

.PHONY: clean all install remove
