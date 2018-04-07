# Ideas

**Alternative pipeline models**. What is a pipeline? (a) -> (b) -> (c). How to model a pipeline? (A -> IO B -> IO C). A pipeline of kernels which each take a set of input registers and memory segments, and write an output segment and output registers. Dividing the pipeline into blocks. Stream kernels. Parallel stream kernels. Completely parallel kernels. Block stream kernels. Big blocks. Little blocks. Strides. SIMT kernels. Buffer swapping

**GeneratePiplineOnGPU** SIMT -> schedule many many things.

Documentation is not vividly clear

**Implementing the multigrep**. Many expressions. Many kernels. Starting point: many kernels for the same expression on the same file. Model needs to let us launch many things. Why not launch many kernels and create many PTX files? Synchronize them with streams (dynamic paralellism; add -lcudart and include cuda_runtime.h)

Can we use co-operative kernels?

( 3x

Create PTX Function
Save PTX Function

)

(

Load PTX Functions
Run PTX Kernels

)

```
cat /dev/urandom | base32 -w 6 | head -n1000 > patterns
# BEFORE PARALLELISM
ubuntu@ip-172-31-46-4:~$ time /tmp/icgrep-5597 -NVPTX -f patterns random
1
1740

real	3m28.739s
user	2m31.548s
sys	0m2.088s
ubuntu@ip-172-31-46-4:~$ time /tmp/icgrep-5597 -NVPTX -f patterns random -re-num 100 -enable-multigrep-kernels
10
1740

real	3m30.380s
user	1m22.968s
sys	0m1.500s
...
```

# Not parallel

An FSA is not completely parallel.

We need to mix logic circuits with bit blocks.

# Profiling

5s to transfer 8 GB twice
