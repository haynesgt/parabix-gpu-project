# Performance Experiments of icgrep with NVPTX Support

## Test Machines Used

<TODO>

## Revisions Used

All NVPTX related tests were performed on a modified version of icgrep at revision 5597 as this revision was the most recent
revision that we could get NVPTX support working in icgrep with minimal changes to the codebase. Any tests for icgrep matching
expressions on the CPU were run on revision 5706.

## Test Data

For the sample data used in performance tests, we opted to use data that would simulate something similar to a spam filter,
a good candidate for running icgrep with NVPTX support on. We generated a random pattern file, consisting of 1000 fixed strings
of length six to simulate a simple filter for spam words. For the data to run icgrep against, we generated a 1GB file by
base64 encoding 1GB of data from `/dev/random` to simulate the raw text from several thousand emails.

## grep

```
(6+ minutes)
```

<TODO> Elaborate with exact command line options used and timing

## icgrep on the CPU

<TODO> Elaborate on the exact commands used

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

## icgrep on the GPU

<TODO> Format this data in tables.

### Without GPU Parallelism

<TODO>

### With GPU Parallelism

<TODO> Explain Stream Context

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

<TODO> Tests with ptx caching?


