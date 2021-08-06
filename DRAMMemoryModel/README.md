Version 2: DRAM Memory Model
===

Find the exact specifications in the [problem statement](https://github.com/pshrey795/MIPS-Simulator/blob/main/DRAMMemoryModel/Statement.pdf).

DRAM Memory
---

* In the last version, we used a very simple implementation of the memory. In actual hardware systems however, the memory is stored as DRAM or Dynamic Random Access Memory which we have implemented as a two dimensional array.
* In practical implementations, the memory is abstracted from the processor, so accessing a particular memory location takes a certain amount of delay. 
* So we need an intermediate buffer(row-buffer) which temporarily stores the most recently accessed row of the DRAM and decreases the delay time. 

Non-blocking Memory
---

* The lw/sw instructions require multiple clock cycles because of the inherent delay in accessing data in the memory or the row-buffer.
* So, these type of memory access operations block or stall the normal instructions causing a large amount of delay.
* We can use the fact, that the memory and processor are separate, to our advantage by considering their execution to be parallel. So, once a request has been issued to the DRAM, we can allow the processor to move on to further instructions without any delay.
* Another plausible optimisation is to store the memory access operations in a queue, so even when there are multiple consecutive requests, the execution doesn't stall.

The complete explanation for this can be found in the [design document](https://github.com/pshrey795/MIPS-Simulator/blob/main/DRAMMemoryModel/Design.pdf).

How to run the code?
---
1. First clone the directory using `git clone https://github.com/pshrey795/MIPS-Simulator.git`.
2. Navigate to the MIPSInterpreter sub-directory and type `make all` in terminal to build the code.
3. There are two different implementations for non-blocking memory as discussed in the document, and both of them can be run separately:
   * For implementation 1, type `./output1 <input_MIPS_code`.
   * For implementation 2, type `./output2 <input_MIPS_code`.
4. The relevent details about the registers and memory will be printed on the console after the program execution.
5. If there is an error in the syntax or semantics, a relevant error message will be printed instead.