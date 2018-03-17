# GPU / NVPTX Parabix - Update 1

## Notes on the GPU

* Thread Hierarchy
  * The GPU is divided into grids
    * Each has a unique grid id (gridid)
    * Each grid is independent
  * Grids are divided into CTAs / cooperative thread arrays (This is in PTX. These are called blocks in CUDA)
    * Each has a unique block id (ctaid)
    * Threads in a block / CTA can synchronize at barriers, and share memory
  * CTAs are divided into warps
    * all threads in the same warp execute the same instruction at the same time
    * Usually 32 threads
    * icgrep uses warps of 64 threads
  * Warps are divided into threads
    * Each thread in a block has a unique thread id (tid)
* Memory Hierarchy: Address spaces
  * Address space 1 is the "global" address space, which is accessable to all threads in all blocks in all grids on the GPU
  * Address space 3 is the "shared" address space, which is accessable to all threads in one block on the GPU
  * Address space 4 is the "constant" address space, which is accessable to all threads on the GPU, read-only
  * Address space 5 is the "local" address space, which is accessable to one thread on the GPU
  * Address space 0 is the "generic" address space, which includes all other address spaces.
    * An address in another space can be converted to a generic address by adding the base address for the smaller address's window.

## Summary of GPU Problems

* GPU support has been broken from revision 5706 onwards.
  * Change 5706 implements major restructuring, affecting thousands of lines of code
  * The GPU / NVPTX pipeline was not updated
  * The problem seems to be that a kernel on the GPU is trying to access the stack memory on the CPU
    * Note, Alloc creates stack memory, Malloc creates heap memory
  * Naive fix attempts don't work (disabling the error check leads to corrupt IR)
  * Address spaces are different
  * Change 5597 adds Malloc for streamset buffers in address space 0, but GPU buffers are created in address space 1
  * Editd creates these GPU buffers:
    * StreamSetBuffer * CCStream = pxDriver.addBuffer(make_unique<SourceBuffer>(iBuilder, iBuilder->getStreamSetTy(4), 1));
    * ExternalBuffer * ResultStream = pxDriver.addExternalBuffer(make_unique<ExternalBuffer>(iBuilder, iBuilder->getStreamSetTy(editDistance+1), resultStreamPtr, 1));
  * GrepEngine creates this GPU buffers:
    * StreamSetBuffer * ByteStream = mGrepDriver->addBuffer(make_unique<SourceBuffer>(idb, idb->getStreamSetTy(1, 8), 1));

## Updating to work with 5597

* We need to create Mallocs for non-generic address spaces, replacing the Alloc, here:
``` {c++}
void StreamSetBuffer::allocateBuffer(const std::unique_ptr<kernel::KernelBuilder> & iBuilder) {
    if (LLVM_LIKELY(mStreamSetBufferPtr == nullptr)) {
        Type * const ty = getType();
        if (mAddressSpace == 0) {
            Constant * size = ConstantExpr::getSizeOf(ty);
            size = ConstantExpr::getMul(size, ConstantInt::get(size->getType(), mBufferBlocks));
            mStreamSetBufferPtr = iBuilder->CreatePointerCast(iBuilder->CreateCacheAlignedMalloc(size), ty->getPointerTo());
        } else {
            mStreamSetBufferPtr = iBuilder->CreateCacheAlignedAlloca(ty, iBuilder->getSize(mBufferBlocks));
        }
        iBuilder->CreateAlignedStore(Constant::getNullValue(ty), mStreamSetBufferPtr, iBuilder->getCacheAlignment());
    } else {
        report_fatal_error("StreamSetBuffer::allocateBuffer() was called twice on the same stream set");
    }
}
```

## LLVM ERROR: stdlib.h does not contain either aligned_alloc or posix_memalign

The problem was that icgrep was constructing buffers in the default address space, where aligned allocs were the default. The function for aligned alloc functions did not exist on the gpu device (posix_memalign or alligned_alloc).


