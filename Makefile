all:
	mkdir -p fab/c/build
	cd fab/c/build && cmake .. && make && make install
	cd sb/ui && make

clean:
	rm -rf fab/c/build
