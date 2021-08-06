Version 3: Memory Request Re-ordering
===

The exact specifications can be found in the [problem statement](https://github.com/pshrey795/MIPS-Simulator/blob/main/MemoryRequestReordering/Statement.pdf).

* In the last version, we saw that replacing the current row in the row-buffer with another row from DRAM Memory takes a significant delay which is not desired. Thus, the delay is less if the switching of row-buffers is infrequent.
* This can be done by reordering the memory access requests such that those accesses which use the same row of the DRAM can be grouped together to minimise the number of switchings required. 
* In this implementation as well, we need to keep track of unsafe instructions, as the memory access operation which is postponed to decrease the delay, may be updating a register which is to be used in subsequent instructions.

The complete explanation for this can be found in the [design document](https://github.com/pshrey795/MIPS-Simulator/blob/main/MemoryRequestReordering/Design.pdf).

How to run the code?
---

1. First clone the directory using `git clone https://github.com/pshrey795/MIPS-Simulator.git`.
2. Navigate to the MemoryRequestReordering sub-directory and type `make all` in terminal to build the code.
3. For execution of the code, type `./output <intput_MIPS_code>` on the console.
4. The relevent details about the registers and memory will be printed for every clock cycle along with the statistics at the end.
5. If there is an error in the syntax or semantics, an appropriate error message will be printed instead.

Testing
---

The testcases are contained in [this folder](https://github.com/pshrey795/MIPS-Simulator/tree/main/MemoryRequestReordering/Testcases). There are various types of testcases which is denoted by the name of that testcase.