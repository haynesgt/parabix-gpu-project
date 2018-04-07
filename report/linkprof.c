// compile with gcc linkprof.c -o linkprof -lcuda -lcudart
// This will compile kernel.ptx and create kernel.cubin
// Run `icgrep -NVPTX xyz /etc/hosts; mv engine.ptx kernel.ptx` to generate a ptx file to test with.

#include <stdlib.h>
#include <stdio.h>

#include <cuda.h>
#include <cuda_runtime.h>

void print_cu_result(CUresult result) {
  const char* s1;
  const char* s2;
  cuGetErrorName(result, &s1);
  cuGetErrorString(result, &s2);
  printf("result = %d %s %s\n", result, s1, s2);
}

int main(int argc, char** argv) {
  CUlinkState state;
  CUresult result;


  // CUresult cuInit ( unsigned int  Flags )
  result = cuInit(0);
  print_cu_result(result);

  CUcontext ctx;
  // CUresult cuCtxCreate ( CUcontext* pctx, unsigned int  flags, CUdevice dev )
  cuCtxCreate(&ctx, 0, 0);
  print_cu_result(result);

  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);

  cudaEventRecord(start, 0);

  // CUresult cuLinkCreate ( unsigned int  numOptions, CUjit_option* options, void** optionValues, CUlinkState* stateOut )
  result = cuLinkCreate(0, NULL, NULL, &state);
  print_cu_result(result);

  // CUresult cuLinkAddFile ( CUlinkState state, CUjitInputType type, const char* path, unsigned int  numOptions, CUjit_option* options, void** optionValues )
  result = cuLinkAddFile(state, CU_JIT_INPUT_PTX, "kernel.ptx", 0, NULL, NULL);
  print_cu_result(result);

  void* cubin;
  size_t size;

  // CUresult cuLinkComplete ( CUlinkState state, void** cubinOut, size_t* sizeOut )
  result = cuLinkComplete(state, &cubin, &size);
  print_cu_result(result);

  FILE* f;
  f = fopen("kernel.cubin", "w");
  fwrite(cubin, size, 1, f);
  fclose(f);

  cudaEventRecord(stop, 0);

  cudaEventSynchronize(stop);

  float milliseconds = 0;
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("Elapsed time: %fms\n", milliseconds);

  // CUresult cuCtxDestroy ( CUcontext ctx )
  result = cuCtxDestroy(ctx);
  print_cu_result(result);

  exit(EXIT_SUCCESS);
}
