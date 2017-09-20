all:
	mkdir -p bin build; cd build; cmake ..; make

clean:
	mkdir bin build; cd build; cmake ..; make clean

mrproper:
	rm -rf bin build

test: all
	script/test/test.sh

bench-graph: all
	script/bench/generate-graph.sh

bench-data: all
	script/bench/generate-data.sh
