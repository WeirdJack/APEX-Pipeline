---------------------------------------------------------------------------------
APEX Pipeline Simulator
---------------------------------------------------------------------------------
A simple implementation of 7 Stage APEX Pipeline based on instructions flow in a 
Computer Architecture

Author :
---------------------------------------------------------------------------------
Bhargav Choudhury (bchoudh4@binghamton.edu)
State University of New York, Binghamton


Notes:
----------------------------------------------------------------------------------
1) This code is a simple implementation of 5 Stage APEX Pipeline. 
	 
	 Fetch -> Decode -> Execute1 -> Execute2 -> Memory1 -> Memory2 -> Writeback
	 
	 You can read, modify and build upon given codebase to add other features as
	 required in project description. You are also free to write your own 
	 implementation from scratch.

2) All the stages have latency of one cycle. There is a single functional unit in 
	 EX stage which perform all the arithmetic and logic operations.

3) Logic to check data dependencies has not be included. You have to implement it.

File-Info
----------------------------------------------------------------------------------
1) Makefile 			- You can edit as needed
2) file_parser.c 	- Contains Functions to parse input file. No need to change this file
3) cpu.c          - Contains Implementation of APEX cpu. You can edit as needed
4) cpu.h          - Contains various data structures declarations needed by 'cpu.c'. You can edit as needed
	 

How to compile and run
----------------------------------------------------------------------------------
1) go to terminal, cd into project directory and type 'make' to compile project
2) Run using ./apex_sim <input file name>
