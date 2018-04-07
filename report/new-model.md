### The CPU and the GPU

Parabix was built with pararellism in mind, but the division of data into small CPU-sized blocks runs deep in the Parabix architecture. These blocks are 128 to 512 bits in size. With the GPU, it is possible to operate on larger blocks. A GPU device can run a trillion operations per second using thousands of cores on gigabyte-sized blocks. This opens up a new world of high-performance processing.

### The Block Orientation

The Parabix model divides the program into kernels which execute on a block of data at a time. Kernels are bound together as producers and consumers. With this view, it makes little difference whether operations are 100% parallel or almost 100% parallel. The 'add' operation on a SIMD register uses few instructions, so it is counted as a parellel operation, even though the fastest add algorithm requires a logarithmic depth circuit. When the program is taken to the GPU where there are thousands of cores, addition programs must use special `ballot` / `vote` instructions, for threads to know where long carries start and where they end.

The ability to use thousands of cores makes clear some issues in the language of Parabix. There is a "pipeline" of kernels that have a "rate" of production and consumption, but it is more correct to talk of a "circuit" of kernels with a "fan-in" and "fan-out". As well, the kernels are in a horizontal pipeline, transferring carries, and a vertical pipeline, transferring layers of data, which is confusing. Since the circuit is very large, on the CPU it is executed sequentially in blocks. With smaller files it is possible to execute entire layers of the circuit in parallel on the GPU. Some kernels, like the popcount kernel, have fewer output bits than input bits, while others, like base64, have more.

### Long Add, Longer Adds

Let's say a *long* add operates on two 64-bit registers, a *long<sup>2</sup>* add operates on two 4096-bit blocks, and a *long<sup>3</sup>* add works on two 262144-bit blocks. The long<sup>2</sup> is easy. Just use a warp of 64 threads to add 64 bit regesters each, then let them use the ballot function to share two bit vectors, one where bit *i* = 1 iff thread *i* generates a carry, and the other where bit *i* = 1 iff thread *i* is all 1's. The threads can then shift the carry vector right by 1 bit and add it to the all-1's ballot, then add 1 if a carry was found to enter the thread's bits.  This is alrady implemented in the Parabix framework. To implement the long<sup>3</sup> add, we can't use the ballot across more than 64 threads, but we can use shared memory to store a list of vectors that store the carry bit and the all-1's flag for each 4096-bit block. Then, each warp of 64 threads can synchronize, use a ballot to quickly turn the shared memory into one carry bit vector and one all-1's bit vector, shift / add them, use another ballot to get the carry and all-1's bit vectors for the warp's own 64 threads, and use the vectors to add 1 if needed, for each thread.

Without using a sequential algorithm, an approach such as the one above is needed for the long<sup>3</sup> add when matches may include thousands of characters. Usually, this is very rare when matching code files that have a newline every few hundred characters, or when matching text divided by periods. The low demand means there is little need to implement it. There are better things to do with the 4096 threads it requires, and some GPU's don't even have enough cores.

### Multigrep, A Better Use for Parallelism

A GPU can search for regex matches a thousand times faster than the file can be transferred to the device. It takes about .3 seconds to transfer a gigabyte to the device. When divided among 1000 threads, the same time gives each thread 300 million cycles to search a one-megabyte section for matches, which is much more than needed for simple expressions. It helps that we use the 64 bit register as a SIMD register of bytes. To make use of the extra time, it is better to use the GPU to search for many patterns in parallel. This is useful for spam filters, search engines, DNA pattern finding, and other text searching.

### Caching Code

The NVPTX runner will simply compile regular expressions into `.ptx` files of PTX assembly in the current directory, which are then compiled into cubin objects and run on the GPU. If the same expression is used multiple times, it will rebuild the objects entirely. It would be better to cache the programs instead of recompiling them each time, and it would be better to store compiled `.cubin` objects instead of `.ptx` assembly. Since build times are high, this would be extremely helpful when a grep program is run in the inner loop of a bash script.

The CPU objects already have caching infrastructure. They use the ParabixObjectCache, which caches individual kernel object files, 
