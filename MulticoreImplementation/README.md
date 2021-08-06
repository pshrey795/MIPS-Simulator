Version 4: Multi-core Implementation (Memory Request Manager)
===

The exact specifications can be found be in the [problem statement](https://github.com/pshrey795/MIPS-Simulator/blob/main/MulticoreImplementation/Statement.pdf).

* All the previous versions dealt with the execution of instructions coming from a single MIPS code file. But now, we extend the same algorithms and optimisations to execute multiple cores at once, each of which deals with a separate input file.
* Although the concept of cores is to introduce parallelism so that the normal instructions of multiple files can be executed at once, there is still only one common DRAM memory and row buffer due to which, memory operation of only a single core can take place on a given clock cycle.
* For that, we require a Memory Request Manager which stores the memory access requests of multiple cores and schedules the next memory operation based on some scheduling algorithm.
* This scheduling algorithm must try to minimize the delay in clock cycles(i.e. maximize throughput) and also take care that no core is starved in the process.

The complete description, algorithm and implementation of this request manager has been explained in detail in the [design document](https://github.com/pshrey795/MIPS-Simulator/blob/main/MulticoreImplementation/Design.pdf)

How to run the code?
---

1. First clone the directory using `git clone https://github.com/pshrey795/MIPS-Simulator.git`.
2. Navigate to the MulticoreImplementation sub-directory and type `make all` in terminal to build the code.
3. For execution of the code, type `./output  <number_of_cores>  <max_time>  <folder_of_input>  <row_access_delay>  <col_access_delay>` on the console.
4. The relevent details about the registers and memory for each core will be printed for every clock cycle along with the statistics at the end.
5. If there is an error in the syntax or semantics in any of the cores, an appropriate error message will be printed instead for those cores.

Testing
---

The testcases are contained in [this folder](https://github.com/pshrey795/MIPS-Simulator/tree/main/MulticoreImplementation/Testcases). There are various types of testcases which is denoted by the name of the folder in which they are grouped.