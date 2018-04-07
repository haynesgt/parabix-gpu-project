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
  
|Streams|Patterns per Stream|CPU Time|GPU Time|
|-------|-------------------|--------|--------|
| 10 | 100 | 14.88s | 60.02s |
| 5 | 200 | 44.69s | 39.79s |
| 20 | 50 | 12.77s | 77.23s|
| 7 | ~150 | 28.69s | 48.92s|
| 14 | ~75 | 13.45 | 63.02s|

<TODO> Tests with ptx caching?


