# Update 2

## Understanding the pipeline in execution

The code generation for the pipeline is in `icgrep/toolchain/pipeline.cpp`

There are 3 pipeline models:

* generateParallelPipeline
* generateSegmentParallelPipeline
* generatePipelineLoop

## The promise for multithreading improvements

1 icgrep-5706 program searching for 1000 simple patterns in a 1GB file:
(several minutes)

10 icgrep-5706 programs to search for 100 simple patterns each, in a 1GB file:

In parallel:
real    0m43.099s
user    5m35.196s
sys     0m0.964s

In series:
real    2m32.088s
user    2m31.496s
sys     0m0.544s

1 grep program:
(6+ minutes)
