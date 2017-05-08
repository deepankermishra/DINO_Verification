# Intermittent_Execution_Verification

The passes are provided in /llvm/lib/Transforms/dino directory which can be executed using LLVM 3.5.0 and Clang 3.5.0.
Sample code files are provided in the folder "Samples".

The command used to build the .bc file for any sample code is:
```bash
$ clang++ -I/home/dell/llvmSrc/llvm/include -I/home/dell/llvmSrc/build/include -std=c++11 -emit-llvm samplex.cpp -c -o samplex.bc
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
$ opt -load /home/dell/llvmSrc/build/lib/LLVMdino.so -dino < samplex.bc > /dev/null
```
