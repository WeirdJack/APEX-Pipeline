Project Description: 
----------------------------------------------------------------------------------------

This project requires you to implement the simulator for an out-of-order processor using register renaming, similar to that of Variation 2 in the lecture notes. The ISA of this processor is identical to the processor for Simulator I.
The specific details of the data path are as follows: 
• The instruction fetch stage and the decoding/renaming stage (D/RN) each have a delay of one cycle. 
• The issue queue (IQ) has a maximum of 8 entries. Register operands are read at the time of issue. It takes one cycle for an IQ entry to wake up, be granted a FU and move to the required function unit. 
• The LSQ has a maximum capacity of 6 entries. 
• The physical register file has 24 registers. Each physical register has an extension that holds any flag values that were generated along with the contents of that register. When the simulation starts, no Fetch D/RN MUL FU: 3 stages Int. FU: 2 stages IQ LSQ ROB MEM: 3 cycle latency, non-pipelined Branch FU: 1 stage 2 of 3 physical registers are allocated. Physical registers are always allocated in increasing order of their addresses. Assume that a physical register that is freed up in a cycle can be reallocated in the same cycle.
• The ROB has a capacity of 12 entries. 
• The function unit for the multiply operations is pipelined into three stages (each with a delay of a single cycle). 
• The integer function unit is pipelined into two stages (each with a delay of single cycle). All integer operations (excluding multiplication), logical operations and address calculations for loads and stores are performed in the integer function unit. 
• The branch function unit has its own adder to calculate a target address and has a single stage (one cycle delay). The issue of a branch instruction checkpoints the allocation list of physical registers and the rename table to speed up recovery on a branch misprediction. 
• Memory operations, once initiated, take 3 cycles to complete and memory and is implemented by a memory function unit as shown. Addresses for the loads and stores are calculated in the integer function unit and written directly to the associated LSQ entry. The write of a calculated memory address to the LSQ takes one cycle, after which the LSQ can begin memory operation as long other conditions for starting memory operations are valid. Memory operations cannot be overlapped. 
• Instructions can be issued from the issue queue to multiple function units simultaneously and the physical register file has sufficient number of ports to allow all register operands for simultaneously issued instructions to be read out in parallel. 
• The processor supports back-to-back execution by implementing wakeup tag broadcasts (from all FUs, including the memory FU) one cycle before the availability of the corresponding result, which can be forwarded to an instruction as it issues. 
• The processor supports speculative execution. A speculation depth of 2 is supported, allowing for at most two unresolved branch instructions at any time. As soon as the branch instruction discovers a mis predicted branch, it takes one cycle to flush all instructions and resume execution along the correct path. A JUMP instruction requires all following instructions that entered the pipeline to be flushed. 

Some other assumptions to be made for designing the simulator are as follows: 
1. A sufficient number of forwarding buses exist to permit simultaneous forwarding from all function units that can forward a result. 
2. If two or more instructions become eligible for issue to the same FU in the same cycle, the instruction with the lowest associated PC value (that is, address) is chosen for issue. (This tie breaking solution is complex to implement in real designs, but we need this to facilitate grading!) 
3. A physical register can be freed up in the same cycle it is written to (and reallocated in the same cycle), as long as all conditions for freeing it up are valid. 
4. An IQ entry is freed up at the end of the cycle in which the instruction it held was issued. 
5. Loads cannot bypass earlier stores. 
6. At the time of dispatching, IQ, ROB and LSQ entries can be set up simultaneously. 
7. Instruction commitment takes one cycle. 
8. Updates to the rename table take place at the beginning of the cycle in which a physical register is being updated, so the dispatching logic is aware of the validity of the physical register.

