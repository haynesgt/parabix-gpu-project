.PHONY: dist

dist: dist/benchmarks/bigbadfile dist/benchmarks/fread dist/benchmarks/mmap

dist/benchmarks/bigbadfile:
	mkdir -p dist/benchmarks
	cat /dev/urandom | base32 -w 31 | head -c`expr 1024 \* 1024 \* 1024` /dev/urandom > dist/benchmarks/bigbadfile

dist/benchmarks/fread:
	mkdir -p dist/benchmarks
	gcc src/benchmarks/fread.c -o dist/benchmarks/fread

dist/benchmarks/mmap:
	mkdir -p dist/benchmarks
	gcc src/benchmarks/mmap.c -o dist/benchmarks/mmap
