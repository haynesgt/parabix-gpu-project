# Project Ideas

* Implement an ICGrep kernel on the GPU (e.g. the scanmatch kernel)
* Implement matching of many patterns against one or many strings, in parallel on the GPU
* Fix and update the editd GPU implementation to compile on modern revisions (and perhaps use CUDA 9 instead of 7.5)

# Examples

## Building a GPU Driver

The current icGrep code generation is done in *grep_engine.cpp*, which creates kernel calls like so:
```C++
mGrepDriver = new ParabixDriver("engine");
auto & idb = mGrepDriver->getBuilder();

...

auto ByteStream = mGrepDriver->addBuffer<SourceBuffer>(idb, idb->getStreamSetTy(1, encodingBits));
auto sourceK = mGrepDriver->addKernelInstance<kernel::FDSourceKernel>(idb);
sourceK->setInitialArguments({fileDescriptor});
mGrepDriver->makeKernelCall(sourceK, {}, {ByteStream});

...
```

For this project, we may implement an NVPTX driver, perhaps to be put in a pipiline before a CPU driver, e.g.:
```C++
gpuDriver = new NVPTXDriver("engine");
auto & idbGPU = gpuDriver->getBuilder();

cpuDriver = new ParabixDriver("scan");
auto & idbCPU = cpuDriver->getBuilder();

...

auto CCStream = gpuDriver->addBuffer<SourceBuffer>(idbGPU idbGPU>getStreamSetTy(4), 1);  
auto sourceK = gpuDriver->addKernelInstance<kernel::MemorySourceKernel>(idbGPU, inputTy, segmentSize);
mGrepGPUDriver->makeKernelCall(sourceK, {}, {CCStream});  

...

// More calls on the GPU or CPU driver.
```

## The GPU editd architecture

### DevOps

* First, install CUDA 7.5: `sudo apt-get install nvidia-cuda-toolkit`

* To build llvm, use this command, which includes the "NVPTX" target:

  `cmake -DCMAKE_INSTALL_PREFIX=../libllvm -DLLVM_TARGETS_TO_BUILD="X86;NVPTX" -DLLVM_BUILD_TOOLS=OFF -DLLVM_BUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=Release ../llvm-3.8.0.src`

* When using cmake to prepare icgrep, enable CUDA by adding `-DENABLE_CUDA_COMPILE=ON`, and I suggest use boost 61

  `cmake -DCMAKE_PREFIX_PATH=../libllvm -DCMAKE_BUILD_TYPE=Release ../icgrep -DENABLE_CUDA_COMPILE=ON -DBOOST_ROOT=/home/ubuntu/boost_1_61_0`

* At the time of writing editd will fail to compile with `-DCUDA_ENABLED` at the most recent revision. Editd does compile with NVPTX support on revision 5584 and so that is what was used for initial testing.

  This will add `-DCUDA_ENABLED` to the compiler command, and adds `-lcuda` to the linker command.
Pragma checks in the C++ files will enable the GPU code generators.

### Command line

Use -NVPTX with editd to use the NVPTX builder

```
echo 127 > /tmp/127
./editd -NVPTX -f /tmp/127 /etc/hosts
Elapsed time : 0.047776 ms
pattern_segs = 1, total_len = 1
total candidate from the first filter is 256
total candidate from the second filter is 252
```

This will read expressions from the file /tmp/127 and output the number of fuzzy matches found in /etc/hosts

### Errors

Running editd with NVPTX support on some machine setups may result in the following error:
```
terminate called after throwing an instance of 'std::runtime_error'
  what():  Kernel interface memory_source_O1 not yet finalized.
Aborted (core dumped)
```
which might indicate that there are possible pipeline generation issues in the NVPTX driver.

### C++

#### call stack

* editd/editd.cpp:main connects buffers, drivers, kernels, and builders
  * block size is always 64
  * GPU program requires input regexes to be from files
  * editd/editd.cpp:editdGPUCodeGen creates NVPTXDriver("editd")
    * SourceBuffer -> MemorySourceKernel -> editdGPUKernel -> ExternalBuffer
    * editd/editd_gpu_kernel.cpp defines editdGPUKernel, which builds the basic blocks
  * editd/editd.cpp:mergeGPUCodeGen creates NVPTXDriver("merge"): creates basic blocks to merge streams
  * IR_Gen/CudaDriver.h:RunPTX uses the CUDA API to launch the kernel
  * editd/editd.cpp:editdScanCPUCodeGen runs a CPU pass (ParabixDriver) on the GPU output

## Performance Benchmarking

* We can compare performance to cat, grep, and cpu icgrep, as well as results from other research papers.
* NVIDIA has a profiler which might give us some interesting results.

### Sample benchmarks:

```
ubuntu@ip-172-31-46-4:~/parabix.costar.sfu.ca/icgrep-devel/icgrep-build-cuda$ time ./icgrep UFOFH ~/random -c -NVPTX
37

real	0m6.194s
user	0m1.072s
sys	0m0.560s
ubuntu@ip-172-31-46-4:~/parabix.costar.sfu.ca/icgrep-devel/icgrep-build-cuda$ time ./icgrep UFOFH ~/random -c -NVPTX
37

real	0m6.028s
user	0m1.048s
sys	0m0.508s
ubuntu@ip-172-31-46-4:~/parabix.costar.sfu.ca/icgrep-devel/icgrep-build-cuda$ time ./icgrep UFOFH ~/random -c
37

real	0m1.011s
user	0m0.952s
sys	0m0.056s
ubuntu@ip-172-31-46-4:~/parabix.costar.sfu.ca/icgrep-devel/icgrep-build-cuda$ time grep UFOFH ~/random -c
37

real	0m1.009s
user	0m0.856s
sys	0m0.152s
ubuntu@ip-172-31-46-4:~/parabix.costar.sfu.ca/icgrep-devel/icgrep-build-cuda$ time cat ~/random > /dev/null

real	0m0.176s
user	0m0.004s
sys	0m0.168s
ubuntu@ip-172-31-46-4:~/parabix.costar.sfu.ca/icgrep-devel/icgrep-build-cuda$ ls -lh ~/random
-rw-rw-r-- 1 ubuntu ubuntu 1.0G Feb 10 00:34 /home/ubuntu/random
ubuntu@ip-172-31-46-4:~/parabix.costar.sfu.ca/icgrep-devel/icgrep-build-cuda$ head ~/random
SL4QHUCH4K4LXJXFCOIO5TEEZH2BWORULLKC2QLI7SWU4XUT3WU7EFCFHSBK5F5
4HQPFGQ6Z2LJRMGR5UERHTHGH27SBLE3GMTGLPQBKPGWMVV6DY7IVUJNAUQEVTK
J5R3WITDUWHWZDOQ55VGR3ZALC55VRYXFU7SAIFKKWNOE2OIJ62HGZRXYMFIULA
J22AZUZ3WKNR5FZOVDZJBENB6ZNQZEICRVWQTQ35CD45LESK24RJBP46KYNKBRH
ISPF5MKJGBHJ4KXWVARWQS5R6UKHOGI4DJSLVWPXBS7PYRJ6HV7C2UPAGF7UMPL
K2BBUGN5ULCRJ5HUKIAKBWL2NYR2GZHBVCQEMHGI4LAVBR55VC5CV7UDMA7VU2R
37UODX6UUJ3IVIPD6DDSRSSM4ZMJQSK3KJOUIHLUEVSFV6GWWBWCZMKBSGYHKG4
J6DAN2G6OOZYVFV3S6KMR42T647M43HZNHTCEXTIZQPG6ZBJLQSC4JAYHHATR6V
NTEBKCH3YCHT7FSWNOBKP5HV3ATI6QUEKCVSFM2RBVHEJCFYMIGQFCU72362M5T
FM26OKSJVM5U7WOWIWPSLWTEBPO2HDV5RVN457GUJXK7E56EQRAHITUUCOP2LHL
```

# Resources

## GPUs

* We have NVIDIA GTX 1080 and 1070 GPUs

* CSIL has NVIDIA GTX 1050 Ti GPUs

* AWS EC2 has GRID K520 GPUs

## Important Articles

* Bitwise Data Parallelism with LLVM: The ICgrep Case Study

  Robert D. Cameron, Nigel Medforth, Dan Lin, Dale Denis, William N. Sumner

  https://www.researchgate.net/profile/Robert_Cameron6/publication/292606865_Bitwise_Data_Parallelism_with_LLVM_The_ICgrep_Case_Study/links/56afa68008ae9f0ff7b26fb3.pdf

* Inductive Doubling Instruction Set Architecture (IDISA): A low-level programming model for cross-platform SIMD programming.

  http://parabix.costar.sfu.ca/wiki/IDISAproject

* Pablo: A Language and Compiler for Parallel Bit Stream Programming

  http://parabix.costar.sfu.ca/wiki/PabloLanguage

## Related Research

* A Hardware Accelerated Regular Expression Matcher

  Manish Burman, Brandon Kase

  http://www.cs.cmu.edu/afs/cs/academic/class/15418-s12/www/competition/bkase.github.com/CUDA-grep/finalreport.html

* GPU-based NFA Implementation for Memory Efficient High Speed Regular Expression Matching

  Yuan Zu, Ming Yang, Zhonghu Xu, Lin Wang, Xin Tian, Kunyang Peng, Qunfeng Dong

  http://home.ustc.edu.cn/~xiaquhet/PDFs/GPUppopp.pdf

* Gregex: GPU based High Speed Regular Expression Matching Engine

  Lei Wang, Shuhui Chen, Yong Tang, Jinshu Su

  https://www.researchgate.net/profile/Shuhui_Chen/publication/221027312_Gregex_GPU_Based_High_Speed_Regular_Expression_Matching_Engine/links/0deec5377726832ee8000000.pdf

* SIMD Parallelization of Applications that Traverse Irregular Data Structures

  Bin Ren, Gagan Agrawal, James R. Larus, Todd Mytkowicz, Tomi Poutanen, Wolfram Schulte

  https://www.researchgate.net/profile/Bin_Ren6/publication/261485082_SIMD_parallelization_of_applications_that_traverse_irregular_data_structures/links/547cfc610cf27ed978623116.pdf

* GPU-accelerated string matching for database applications

  Evangelia A. Sitaridi1, Kenneth A. Ross

  https://link.springer.com/content/pdf/10.1007%2Fs00778-015-0409-y.pdf

* Fast Regular Expression Matching with Bit-parallel Data Streams

  Robert D. Cameron, Kenneth S. Herdy, Ben Hull, Thomas C. Shermer

  http://parabix.costar.sfu.ca/export/3465/docs/Working/re/re-main.pdf

* High-Performance Regular Expression Scanning on the Cell/B.E. Processor

  Daniele Paolo Scarpazza, Gregory F. Russell

  http://delivery.acm.org/10.1145/1550000/1542284/p14-scarpazza.pdf?ip=142.58.15.125&id=1542284&acc=ACTIVE%20SERVICE&key=FD0067F557510FFB%2E87ED2FCA0B39BEF5%2E4D4702B0C3E38B35%2E4D4702B0C3E38B35&__acm__=1518033609_70ebd48f5b617cdab25b1e4467c242a9
  
* Performance Analysis of Hybrid CPU/GPU Environments

  Michael Shawn Smith

  https://pdxscholar.library.pdx.edu/cgi/viewcontent.cgi?article=1299&context=open_access_etds

* The Landscape of Parallel Computing Research: A View from Berkeley

  Krste Asanovíc, Rastislav Bodik, Bryan Catanzaro, Joseph Gebis, Parry Husbands, Kurt Keutzer, David Patterson, William Plishker, John Shalf, Samuel Williams, and Katherine Yelick

  https://courses.cs.washington.edu/courses/cse549/07wi/files/EECS-2006-183.pdf

## Programming Resources

* CUDA Toolkit Documentation v7.5

  /usr/share/doc/nvidia-cuda-doc/html
  
  https://web.archive.org/web/20160627124508/http://docs.nvidia.com/cuda/
  
* User Guide for NVPTX Back-end

  https://llvm.org/docs/NVPTXUsage.html
