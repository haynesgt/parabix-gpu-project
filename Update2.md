# Update 2

## Understanding the pipeline in execution

The code generation for the pipeline is in `icgrep/toolchain/pipeline.cpp`

There are 3 pipeline models:

* generatePipelineLoop
* generateParallelPipeline
* generateSegmentParallelPipeline

The NVPTX uses generatePipelineLoop. This method just creates one synchronous program for each part (1/256th) of the file.

## Utilising CUDA Streams

The NVPTX implementation in icgrep-5706 performs synchronous function calls on the default stream, so the obvious first step is to see what happens if we divide the work and process the work concurrently. For CUDA applications, this is done by specifying *streams* to run specific functions on.

Due to the way the NVPTXDriver is currently implemented, we divided the regular expression work by creating a different ptx file for each group of work to be run in parallel. Then for each ptx file we obtain the main function and run it on a separate CUDA stream.

## The promise for multithreading improvements

1 icgrep-5706 program searching for 1000 simple patterns in a 1GB file:
```
(several minutes)
```

10 icgrep-5706 programs to search for 100 simple patterns each, in a 1GB file:
In parallel:
```
real    0m43.099s
user    5m35.196s
sys     0m0.964s
```

In series:
```
real    2m32.088s
user    2m31.496s
sys     0m0.544s
```

1 grep program:
```
(6+ minutes)
```

The following NVPTX related tests were run on a GTX 960.

1 icgrep-5706 program with NVPTX support, running over 10 parallel streams. Each stream searching for 100 patterns out of 1000 in a 1GB file:
```
real    1m17.41s
user    14.88s
sys     0.97s

CUDA    60016.199219 ms
```

NVPTX icgrep, running over 5 parallel streams. Each stream searching for 200 patterns:
```
real    1m02.67s
user    44.69s
sys     9.85s

CUDA    39787.601562 ms
```

20 parallel streams. 50 patterns per stream:
```
real    1m30.62s
user    12.77s
sys     0.59s

CUDA    77227.875000 ms
```

7 parallel streams. ~150 patterns per stream:
```
real    1m08.23s
user    28.60s
sys     4.87s

CUDA    48923.113281 ms
```

14 parallel streams. ~75 patterns per stream:
```
real    1m17.10s
user    13.45s
sys     0.58s

CUDA    63023.867188 ms
```
