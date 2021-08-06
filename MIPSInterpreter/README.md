Version 1: MIPS ISA Interpreter
===

The exact specifications can be found be in the [problem statement](https://github.com/pshrey795/MIPS-Simulator/blob/main/MIPSInterpreter/Statement.pdf).

* C++ program which interprets a subset of the MIPS instruction set architecture.
* It takes a code written in MIPS as input and interprets it by maintaining internal data structures to store processor components like Register File and Memory.
* Because this is just a basic implementation, the memory is represented as a linear block i.e. an array.

For detailed description of design decisions, underlying algorithm and implementation, refer [this](https://github.com/pshrey795/MIPS-Simulator/blob/main/MIPSInterpreter/Design.pdf).

How to run the code?
---

1. First clone the directory using `git clone https://github.com/pshrey795/MIPS-Simulator.git`.
2. Navigate to the MIPSInterpreter sub-directory and type `make all` in terminal to build the code.
3. To run the interpreter, type `./output  <intput_MIPS_code>` in the terminal.
4. The relevent details about the registers and memory will be printed on the console after the program execution.
5. If there is an error in the syntax or semantics, a relevant error message will be printed instead.

Testing
---

The testcases, both the valid(to check execution) and invalid(to check error handling), are contained in [this folder](https://github.com/pshrey795/MIPS-Simulator/tree/main/MIPSInterpreter/Testcases). 