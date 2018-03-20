Desired:

* kernels that share data need to run on warps where locality is high.

The state of things:

* gridDimX is GroupNum, which is 256 by default
* blockSizeX is 64.
