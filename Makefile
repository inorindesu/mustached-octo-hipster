
build: main.c
	mkdir -p ./build/
	gcc -std=c99 `pkg-config --libs --cflags freetype2` main.c -o ./build/ftOutlineTest

.PHONY: clean

clean:
	rm -r ./build/
