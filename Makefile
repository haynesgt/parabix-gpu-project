.PHONY: dist

dist: dist/benchmarks/bigbadfile dist/benchmarks/fread dist/benchmarks/mmap

dist/benchmarks/bigbadfile:
	mkdir -p dist/benchmarks
	cat /dev/urandom | base32 -w 31 | head -c`expr 1024 \* 1024 \* 1024` > dist/benchmarks/bigbadfile

dist/benchmarks/fread: src/benchmarks/fread.c
	mkdir -p dist/benchmarks
	gcc src/benchmarks/fread.c -o dist/benchmarks/fread

dist/benchmarks/mmap: src/benchmarks/mmap.c
	mkdir -p dist/benchmarks
	gcc src/benchmarks/mmap.c -o dist/benchmarks/mmap
