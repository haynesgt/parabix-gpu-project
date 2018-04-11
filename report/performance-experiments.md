# Performance Experiments of icgrep with NVPTX Support

## Test Machines Used

NVPTX related tests were performed on two separate machines:
* One local machine containing
    * An Intel i7 6700k CPU running at 4.4GHz
    * One NVIDIA GTX 960 GPU
* One remote Amazon p2.xlarge instance
    * Runs on Intel Xeon E5-2686 v4 CPUs
    * CUDA functions running on NVIDIA K80 GPUs

## Revisions Used

All NVPTX related tests were performed on a modified version of icgrep at revision 5597 as this revision was the most recent
revision that we could get NVPTX support working in icgrep with minimal changes to the codebase. Any tests for icgrep matching expressions on the CPU were run on revision 5706.

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

The default NVPTX implementation in icgrep performs synchronous function calls on the default stream, our tests attempted to  see what happens to performance if we divide the work and process the work concurrently. For CUDA applications, this is done by specifying streams to run specific functions on.

We divided the regular expression work by creating a different ptx file for each group of regular expressions to be run in parallel. Then for each ptx file we obtain the main function and run it on a separate CUDA stream.

The tests were run as follows on the local machine:
`icgrep -f <pattern_file> <sample_data> -re-num <pps> -enable-multigrep-kernels`

Where `pattern_file` is the generated set of patterns specified above, `sample_data` is the sample data specified above, and
`pps` is the maximum number of patterns per stream which is equal to the maximum number of regular expressions in a single group.
  
|Streams|Patterns per Stream|CPU Time|GPU Time|
|-------|-------------------|--------|--------|
| 1 | 1000 | 112.59s | 62.52s |
| 2 | 500 | 86.29s | 45.87s |
| 4 | 250 | 53.79s | 36.69s |
| 5 | 200 | 44.69s | 39.79s |
| 7 | 150 | 28.69s | 48.92s|
| 10 | 100 | 14.88s | 60.02s |
| 14 | 75 | 13.45 | 63.02s |
| 20 | 50 | 12.77s | 77.23s |

There is a clear correlation between the maximum number of patterns per stream and the CPU time spent processing the kernels into ptx files, loading said files into modules, and moving data to and from the GPU.

### Reducing CPU Time with a PTX Cache

From the previous NVPTX test results there are three major causes of long CPU time when running icgrep on the GPU:
1. Creating the various kernels to process into ptx files
2. Loading multiple ptx files and loading them into CUDA modules
3. Moving large amounts of data to and from the GPU

The first issue can be removed by implementing a cache for the ptx files such that they are not run on subsequent invokations of icgrep.

### Interleaving Loading and Running PTX
