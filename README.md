# MIPS Simulator

This directory contains the implementation of a fully functional simulator for MIPS instruction set architecture written in C++. I have also included some basic programs written in MIPS assembly language. The simulator itself has been divided into four stages of increasing functionality and parallelism. The four stages are:

1. **Stage-1:** Basic MIPS ISA Interpreter
2. **Stage-2:** MIPS ISA Interpreter with DRAM memory model with non-blocking flow of execution.
3. **Stage-3:** MIPS ISA Interpreter with DRAM Memory Request Reordering.
4. **Stage-4:** MIPS ISA Interpreter with Memory Request Manager to support multiple cores.