all:
	mkdir -p fab/c/build
	cd fab/c/build && cmake .. && make && make install

clean:
	rm -rf fab/c/build
