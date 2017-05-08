# Intermittent_Execution_Verification

The pass is provided in /code directory which can be executed using LLVM 3.5.0 and Clang 3.5.0.
Test cases are provided in /TestCases.

The command used to build the .bc file for any sample code is:
```bash
$ clang++ -I/home/dell/llvmsrc/llvm/include -I/home/dell/llvmsrc/build/include -std=c++11 -emit-llvm testx.cpp -c -o testx.bc
```
The above instruction is assuming that the source is located in /home/dell directory.

## To run LLVM passes:
1) First run make in the build directory of your LLVM source. Using:
```bash
$ make
```
this generates the .so file for your pass.

2) Now to run the .so on your sample code using:
```bash
$ opt -load /home/dell/llvmSrc/build/lib/LLVMliveness.so -liveness < testx.bc > /dev/null
```
