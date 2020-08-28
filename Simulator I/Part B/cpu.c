/*
 *  cpu.c
 *  Contains APEX cpu pipeline implementation
 *
 *  Author :
 *  Bhargav Choudhury (bchoudh4@binghamton.edu)
 *  State University of New York, Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "cpu.h"

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1

int BZ_Flag;

int stageEX1 = 1;
int stageEX2 = 1;
int stageMEM1 = 1;
int stageMEM2 = 1;

int register_num = INT_MIN;

/*
 * This function creates and initializes APEX cpu.
 *
 * Note : You are free to edit this function according to your
 * 				implementation
 */
APEX_CPU*
APEX_cpu_init(const char* filename)
{
  if (!filename) {
    return NULL;
  }

  APEX_CPU* cpu = malloc(sizeof(*cpu));
  if (!cpu) {
    return NULL;
  }

  /* Initialize PC, Registers and all pipeline stages */
  cpu->pc = 4000;
  memset(cpu->regs, 0, sizeof(int) * 32);
  memset(cpu->regs_valid, 1, sizeof(int) * 32);
  memset(cpu->stage, 0, sizeof(CPU_Stage) * NUM_STAGES);
  memset(cpu->data_memory, 0, sizeof(int) * 4000);

  /* Parse input file and create code memory */
  cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);

  if (!cpu->code_memory) {
    free(cpu);
    return NULL;
  }

  if (ENABLE_DEBUG_MESSAGES) {
    fprintf(stderr,
            "APEX_CPU : Initialized APEX CPU, loaded %d instructions\n",
            cpu->code_memory_size);
    fprintf(stderr, "APEX_CPU : Printing Code Memory\n");
    printf("%-9s %-9s %-9s %-9s %-9s\n", "opcode", "rd", "rs1", "rs2", "imm");

    for (int i = 0; i < cpu->code_memory_size; ++i) {

      if(cpu->code_memory[i].rd > register_num){

        register_num = cpu->code_memory[i].rd;
      }

      if(cpu->code_memory[i].rs1 > register_num){

        register_num = cpu->code_memory[i].rs1;
      }

      if(cpu->code_memory[i].rs2 > register_num){

        register_num = cpu->code_memory[i].rs2;
      }
      printf("%-9s %-9d %-9d %-9d %-9d\n",
             cpu->code_memory[i].opcode,
             cpu->code_memory[i].rd,
             cpu->code_memory[i].rs1,
             cpu->code_memory[i].rs2,
             cpu->code_memory[i].imm);
    }
  }

  /* Make all stages busy except Fetch stage, initally to start the pipeline */
  /*for (int i = 1; i < NUM_STAGES; ++i) {
    cpu->stage[i].busy = 1;
  }*/

  return cpu;
}

/*
 * This function de-allocates APEX cpu.
 *
 * Note : You are free to edit this function according to your
 * 				implementation
 */
void
APEX_cpu_stop(APEX_CPU* cpu)
{
  free(cpu->code_memory);
  free(cpu);
}

/* Converts the PC(4000 series) into
 * array index for code memory
 *
 * Note : You are not supposed to edit this function
 *
 */
int
get_code_index(int pc)
{
  return (pc - 4000) / 4;
}

static void
print_instruction(CPU_Stage* stage)
{
  if (strcmp(stage->opcode, "STORE") == 0) {
    printf(
      "%s,R%d,R%d,#%d ", stage->opcode, stage->rs1, stage->rs2, stage->imm);
  }

  if (strcmp(stage->opcode, "STR") == 0) {
    printf(
      "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "LOAD") == 0) {
    printf(
      "%s,R%d,R%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
  }

  if (strcmp(stage->opcode, "LDR") == 0) {
    printf(
      "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "MOVC") == 0) {
    printf("%s,R%d,#%d ", stage->opcode, stage->rd, stage->imm);
  }

  if (strcmp(stage->opcode, "ADD") == 0) {
    printf(
      "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "ADDL") == 0) {
    printf(
      "%s,R%d,R%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
  }

  if (strcmp(stage->opcode, "SUB") == 0) {
    printf(
      "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "SUBL") == 0) {
    printf(
      "%s,R%d,R%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
  }

  if (strcmp(stage->opcode, "MUL") == 0) {
    printf(
      "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "BZ") == 0) {
    printf(
      "%s,#%d ", stage->opcode, stage->imm);
  }

  if (strcmp(stage->opcode, "AND") == 0) {
    printf("%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "OR") == 0) {
    printf("%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "EX-OR") == 0) {
    printf("%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "HALT") == 0){
    printf("HALT");
  }

  if (strcmp(stage->opcode, "JUMP") == 0) {
    printf("%s,R%d,#%d", stage->opcode,stage->rs1,stage->imm);
  }

  if (strcmp(stage->opcode, "BNZ") == 0) {
    printf("%s,#%d", stage->opcode,stage->imm);
  }
}

/* Debug function which dumps the cpu stage
 * content
 *
 * Note : You are not supposed to edit this function
 *
 */
static void
print_stage_content(char* name, CPU_Stage* stage)
{
  printf("%-15s: pc(%d) ", name, stage->pc);
  print_instruction(stage);
  printf("\n");
}

void make_stage_empty(CPU_Stage *stage) {
    strcpy(stage->opcode, "");
    stage->pc = 0;
}

void make_register_valid(APEX_CPU* cpu, CPU_Stage *stage) {
    
    cpu->regs_valid[stage->rd] = 1;
    cpu->regs_valid[stage->rs1] = 1;
    cpu->regs_valid[stage->rs2] = 1;
}

/*
 *  Fetch Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
fetch(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[F];

  if (!stage->busy && !stage->stalled) {  

    /* Store current PC in fetch latch */
    stage->pc = cpu->pc;

    /* Index into code memory using this pc and copy all instruction fields into
     * fetch latch
     */
    APEX_Instruction* current_ins = &cpu->code_memory[get_code_index(cpu->pc)];
    strcpy(stage->opcode, current_ins->opcode);
    stage->rd = current_ins->rd;
    stage->rs1 = current_ins->rs1;
    stage->rs2 = current_ins->rs2;
    stage->imm = current_ins->imm;


    if (cpu->stage[DRF].stalled == 1) {
            if (ENABLE_DEBUG_MESSAGES) {
                print_stage_content("Fetch", stage);
            }
            return 0;
        }

    /* Update PC for next instruction */
    cpu->pc += 4;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Fetch", stage);
    }

    /* Copy data from fetch latch to decode latch*/
    cpu->stage[DRF] = cpu->stage[F];
  } 

  else{

  	if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Fetch", stage);
    }
  }

  return 0;
}

/*
 *  Decode Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
decode(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[DRF];

  if (!stage->busy && !stage->stalled) {


    /* Read data from register file for store */
    if (strcmp(stage->opcode, "STORE") == 0) {

    	if(cpu->regs_valid[stage->rs1] == 0 || cpu->regs_valid[stage->rs2] == 0){

  		} else{
			stage->stalled = 0;
			stage->rs1_value = cpu->regs[stage->rs1];
        	stage->rs2_value = cpu->regs[stage->rs2];
  		}
    }

    if (strcmp(stage->opcode, "BZ") == 0) {

      APEX_Instruction* current = &cpu->code_memory[get_code_index(cpu->pc - 8)];


      if(strcmp(current->opcode, "SUB") == 0 || strcmp(current->opcode, "ADDL") == 0 || strcmp(current->opcode, "ADD") == 0 || strcmp(current->opcode, "SUBL") == 0 || strcmp(current->opcode, "MUL") == 0){

        if(stageEX1 == stage->pc){
          
          stage->stalled = 0;
          cpu->stage[EX1] = cpu->stage[DRF];
            if (ENABLE_DEBUG_MESSAGES) {
              print_stage_content("Decode/RF", stage);
            }

            return 0;

        }else{
            stage->stalled = 1;
            cpu->stage[EX1] = cpu->stage[DRF];
            if (ENABLE_DEBUG_MESSAGES) {
              print_stage_content("Decode/RF", stage);
            }

            return 0;
        }      
        }        
    }

    if (strcmp(stage->opcode, "BNZ") == 0) {

      APEX_Instruction* current = &cpu->code_memory[get_code_index(cpu->pc - 8)];


      if(strcmp(current->opcode, "SUB") == 0 || strcmp(current->opcode, "ADDL") == 0 || strcmp(current->opcode, "ADD") == 0 || strcmp(current->opcode, "SUBL") == 0 || strcmp(current->opcode, "MUL") == 0){

        if(stageEX1 == stage->pc){
          
          stage->stalled = 0;
          
            if (ENABLE_DEBUG_MESSAGES) {
              print_stage_content("Decode/RF", stage);
            }

            cpu->stage[EX1] = cpu->stage[DRF];

            return 0;

        }else{
          stage->stalled = 1;
            
            if (ENABLE_DEBUG_MESSAGES) {
              print_stage_content("Decode/RF", stage);
            }

            cpu->stage[EX1] = cpu->stage[DRF];

            return 0;
        }      
        }        
    }

    /* Read data from register file for store */
    if (strcmp(stage->opcode, "STR") == 0) {

    	if(cpu->regs_valid[stage->rs1] == 0 || cpu->regs_valid[stage->rs2] == 0){
			
		   if(stage->rd == cpu->stage[MEM1].rs1){
             cpu->regs[stage->rd] = cpu->stage[MEM1].buffer;
           }

           if(stage->rd == cpu->stage[MEM2].rs1){
             cpu->regs[stage->rd] = cpu->stage[MEM2].buffer;
           }

           if(stage->rd == cpu->stage[MEM1].rs2){
             cpu->regs[stage->rd] = cpu->stage[MEM1].buffer;
           }

           if(stage->rd == cpu->stage[MEM2].rs2){
             cpu->regs[stage->rd] = cpu->stage[MEM2].buffer;
           }

  		} else{
			stage->stalled = 0;
			stage->rs1_value = cpu->regs[stage->rs1];
        	stage->rs2_value = cpu->regs[stage->rs2];
  		}
    }

    /* No Register file read needed for MOVC */
    if (strcmp(stage->opcode, "MOVC") == 0) {

    	if(cpu->regs_valid[stage->rd] == 0){
			if(stage->rd == cpu->stage[EX2].rd){
             stage->rd = cpu->stage[EX2].buffer;
		   }
  		} else{
			stage->stalled = 0;
			cpu->regs_valid[stage->rd] = 0;
  		}
    }

    if (strcmp(stage->opcode, "ADD") == 0) {

    	if(cpu->regs_valid[stage->rs1] == 0 || cpu->regs_valid[stage->rs2] == 0){

		   if(stage->rs1 == cpu->stage[EX2].rd){
             stage->rs1_value = cpu->stage[EX2].buffer;
		   }

           if(stage->rs2 == cpu->stage[EX2].rd){
             stage->rs2_value = cpu->stage[EX2].buffer;
           }

  		} else{
			stage->stalled = 0;
			stage->rs1_value = cpu->regs[stage->rs1];
        stage->rs2_value = cpu->regs[stage->rs2];
        cpu->regs_valid[stage->rd] = 0;
  		}
    	
    }

    if (strcmp(stage->opcode, "AND") == 0) {

    	if(cpu->regs_valid[stage->rs1] == 0 || cpu->regs_valid[stage->rs2] == 0){

			   if(stage->rs1 == cpu->stage[MEM1].rd){

            stage->rs1_value = cpu->stage[MEM1].buffer;

          }else if(stage->rs1 == cpu->stage[MEM2].rd){

            stage->rs1_value = cpu->stage[MEM2].buffer;
        
          }else{

            stage->stalled = 1;
          }

          if(stage->rs2 == cpu->stage[MEM1].rd){

            stage->rs2_value = cpu->stage[MEM1].buffer;

          }else if(stage->rs2 == cpu->stage[MEM2].rd){

            stage->rs2_value = cpu->stage[MEM2].buffer;
        
          }else{

            stage->stalled = 1;
          }
           
  		} else{
			   stage->stalled = 0;
			   stage->rs1_value = cpu->regs[stage->rs1];
          stage->rs2_value = cpu->regs[stage->rs2];
        cpu->regs_valid[stage->rd] = 0;
  		}
    	
    }

    if (strcmp(stage->opcode, "SUB") == 0) {

    	if(cpu->regs_valid[stage->rs1] == 0 || cpu->regs_valid[stage->rs2] == 0){
			

    		
			if(stage->rs1 == cpu->stage[EX2].rd){
             stage->rs1_value = cpu->stage[EX2].buffer;
		   }

           if(stage->rs2 == cpu->stage[EX2].rd){
             stage->rs2_value = cpu->stage[EX2].buffer;
           }

  		
  		} else{
			stage->stalled = 0;
			stage->rs1_value = cpu->regs[stage->rs1];
        stage->rs2_value = cpu->regs[stage->rs2];
        cpu->regs_valid[stage->rd] = 0;
  		}
    }

    if (strcmp(stage->opcode, "MUL") == 0) {

    	if(cpu->regs_valid[stage->rs1] == 0 || cpu->regs_valid[stage->rs2] == 0){
			
    		if(stage->rs1 == cpu->stage[EX2].rd){
             stage->rs1_value = cpu->stage[EX2].buffer;
		   }

           if(stage->rs2 == cpu->stage[EX2].rd){
             stage->rs2_value = cpu->stage[EX2].buffer;
           }

  		} else{
			stage->stalled = 0;
			stage->rs1_value = cpu->regs[stage->rs1];
        stage->rs2_value = cpu->regs[stage->rs2];
        cpu->regs_valid[stage->rd] = 0;
  		}
    }

    /* Read data from register file for Load */
    if (strcmp(stage->opcode, "LOAD") == 0) {


	   if(cpu->regs_valid[stage->rs1] == 0/* || cpu->regs_valid[stage->rd] == 0*/){
			
			if(stage->rs1 == cpu->stage[EX2].rd){
             stage->rs1_value = cpu->stage[EX2].buffer;
           }

           if(stage->rs1 == cpu->stage[MEM1].rd){
             stage->rs1_value = cpu->stage[MEM1].buffer;
           }

           if(stage->rs1 == cpu->stage[MEM2].rd){
             stage->rs1_value = cpu->stage[MEM2].buffer;
           }

           if(stage->rs2 == cpu->stage[EX2].rd){
             stage->rs2_value = cpu->stage[EX2].buffer;
           }

           if(stage->rs2 == cpu->stage[MEM1].rd){
             stage->rs2_value = cpu->stage[MEM1].buffer;
           }

           if(stage->rs2 == cpu->stage[MEM2].rd){
             stage->rs2_value = cpu->stage[MEM2].buffer;
           }

  		} else{
			stage->stalled = 0;
			stage->rs1_value = cpu->regs[stage->rs1];
        
        cpu->regs_valid[stage->rd] = 0;
  		}
    }

    /* Read data from register file for LDR */
    if (strcmp(stage->opcode, "LDR") == 0) {
    	
	   if(cpu->regs_valid[stage->rs1] == 0/* || cpu->regs_valid[stage->rd] == 0*/){

		   if(stage->rs1 == cpu->stage[EX2].rd){
             stage->rs1_value = cpu->stage[EX2].buffer;
           }

           if(stage->rs1 == cpu->stage[MEM1].rd){
             stage->rs1_value = cpu->stage[MEM1].buffer;
           }

           if(stage->rs1 == cpu->stage[MEM2].rd){
             stage->rs1_value = cpu->stage[MEM2].buffer;
           }

           if(stage->rs2 == cpu->stage[EX2].rd){
             stage->rs2_value = cpu->stage[EX2].buffer;
           }

           if(stage->rs2 == cpu->stage[MEM1].rd){
             stage->rs2_value = cpu->stage[MEM1].buffer;
           }

           if(stage->rs2 == cpu->stage[MEM2].rd){
             stage->rs2_value = cpu->stage[MEM2].buffer;
           }

  		} else{
			stage->stalled = 0;
			stage->rs1_value = cpu->regs[stage->rs1];
        stage->rs2_value = cpu->regs[stage->rs2];
        cpu->regs_valid[stage->rd] = 0;
  		}
    }

    /* Read data from register file for Or */
    if (strcmp(stage->opcode, "OR") == 0) {
	   if(cpu->regs_valid[stage->rs1] == 0 || cpu->regs_valid[stage->rs2] == 0){
			
			if(stage->rs1 == cpu->stage[EX2].rd){
             stage->rs1_value = cpu->stage[EX2].buffer;
		   }

           if(stage->rs2 == cpu->stage[EX2].rd){
             stage->rs2_value = cpu->stage[EX2].buffer;
           }
  		} else{
			stage->stalled = 0;
			stage->rs1_value = cpu->regs[stage->rs1];
        stage->rs2_value = cpu->regs[stage->rs2];
        cpu->regs_valid[stage->rd] = 0;
  		}
	  }
  

    /* Read data from register file for Ex-Or */
    if (strcmp(stage->opcode, "EX-OR") == 0) {
	   if(cpu->regs_valid[stage->rs1] == 0 || cpu->regs_valid[stage->rs2] == 0){

			if(stage->rs1 == cpu->stage[EX2].rd){
             stage->rs1_value = cpu->stage[EX2].buffer;
		   }

           if(stage->rs2 == cpu->stage[EX2].rd){
             stage->rs2_value = cpu->stage[EX2].buffer;
           }
  		} else{

			stage->stalled = 0;
			stage->rs1_value = cpu->regs[stage->rs1];
        	stage->rs2_value = cpu->regs[stage->rs2];
        	cpu->regs_valid[stage->rd] = 0;
  		}
     
    }

    if (strcmp(stage->opcode, "HALT") == 0) {}

    

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Decode/RF", stage);
    }

    cpu->stage[EX1] = cpu->stage[DRF];
} else{
	if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Decode/RF", stage);
    }
}

 
  return 0;
}

/*
 *  Execute Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
execute1(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[EX1];

  if(cpu->stage[DRF].rs1 == cpu->stage[EX2].rd || cpu->stage[DRF].rs2 == cpu->stage[EX2].rd){

    cpu->stage[DRF].stalled = 0;

  }else if(cpu->stage[DRF].rs1 == cpu->stage[MEM1].rd || cpu->stage[DRF].rs2 == cpu->stage[MEM1].rd){

    cpu->stage[DRF].stalled = 0;
  }else if(cpu->stage[DRF].rs1 == cpu->stage[MEM2].rd || cpu->stage[DRF].rs2 == cpu->stage[MEM2].rd){

    cpu->stage[DRF].stalled = 0;
  }

  stageEX1 = stage->pc;

  if (!stage->busy && !stage->stalled) {

  	if (strcmp(stage->opcode, "HALT") == 0) {
	   
	  	
	  	cpu->stage[F].stalled = 1;
	  	make_stage_empty(&cpu->stage[F]);
	  	make_stage_empty(&cpu->stage[DRF]);
	  	cpu->stage[EX2] = cpu->stage[EX1];
	  	if (ENABLE_DEBUG_MESSAGES) {
      		print_stage_content("Execute1", stage);
    	}
	  	return 0;
     
    }

    /* STORE */
    if(strcmp(stage->opcode, "STORE") == 0) {
      stage->mem_address = stage->rs2_value + stage->imm;
    }

    if(strcmp(stage->opcode, "STR") == 0) {
      stage->mem_address = stage->rs1_value + stage->rs2_value;
    }

    /* MOVC */
    if(strcmp(stage->opcode, "MOVC") == 0) {

      stage->buffer = stage->imm;
    }

    if(strcmp(stage->opcode, "LOAD") == 0) {

      stage->mem_address = stage->rs1_value + stage->imm;
    }

    if(strcmp(stage->opcode, "LDR") == 0) {

      stage->mem_address = stage->rs1_value + stage->rs2_value;
    }

    if (strcmp(stage->opcode, "ADD") == 0) {

    	stage->buffer = stage->rs1_value + stage->rs2_value;
    }

    if (strcmp(stage->opcode, "ADDL") == 0) {

    	stage->buffer = stage->rs1_value + stage->imm;
           
    }

    if (strcmp(stage->opcode, "SUB") == 0) {

    	stage->buffer = stage->rs1_value - stage->rs2_value;

    }

    if (strcmp(stage->opcode, "SUBL") == 0) {

    	stage->buffer = stage->rs1_value - stage->imm;

    }

    if (strcmp(stage->opcode, "MUL") == 0) {

    	stage->buffer = stage->rs1_value * stage->rs2_value;

    }

    if (strcmp(stage->opcode, "OR") == 0) {

      stage->buffer = stage->rs1_value | stage->rs2_value;
    }

    if (strcmp(stage->opcode, "EX-OR") == 0) {

      stage->buffer = stage->rs1_value ^ stage->rs2_value;
    }

    if (strcmp(stage->opcode, "AND") == 0) {

      stage->buffer = stage->rs1_value&stage->rs2_value;
    }

    if (strcmp(stage->opcode, "BZ") == 0) {

    	//stage->stalled = 0;

        if (BZ_Flag == 0) {
              stage->buffer = stage->pc + stage->imm;             
                
        } else {

                stage->buffer = cpu->pc + 8;
        }
    }

    if (strcmp(stage->opcode, "JUMP") == 0) {

        stage->buffer = stage->pc + stage->imm;
    }
        

    if (strcmp(stage->opcode, "BNZ") == 0) {

    	if (BZ_Flag == 1) {
              stage->buffer = stage->pc + stage->imm;             
                
        } else {
        	
                stage->buffer = cpu->pc + 8;
        }
        
    }        

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Execute1", stage);
    }
    cpu->stage[EX2] = cpu->stage[EX1];

    
}

else{

	if (strcmp(cpu->stage[DRF].opcode, "BZ") == 0) {

    	cpu->stage[DRF].stalled = 0;
    }

	make_stage_empty(stage);
	if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Execute1", stage);
    }
    cpu->stage[EX2] = cpu->stage[EX1];
}
  return 0;
}

int
execute2(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[EX2];

  stageEX2 = stage->pc;

  if (!stage->busy && !stage->stalled) {

  	if (strcmp(stage->opcode, "HALT") == 0) {}

    /* STORE */
    /*if(strcmp(stage->opcode, "STORE") == 0) {

    	stage->buffer = cpu->stage[EX1].rd;
    }

    if(strcmp(stage->opcode, "STR") == 0) {

    	stage->buffer = cpu->stage[EX1].rd;
    }

    
    if(strcmp(stage->opcode, "MOVC") == 0) {
      
    	stage->buffer = cpu->stage[EX1].rd;
    }

    if(strcmp(stage->opcode, "LOAD") == 0) {
      
    	stage->buffer = cpu->stage[EX1].rd;
    }

    if(strcmp(stage->opcode, "LDR") == 0) {
      
    	stage->buffer = cpu->stage[EX1].rd;
    }

    if (strcmp(stage->opcode, "ADD") == 0) {

    	stage->buffer = cpu->stage[EX1].rd;
    }

    if (strcmp(stage->opcode, "ADDL") == 0) {

    	stage->buffer = cpu->stage[EX1].rd;
    }

    if (strcmp(stage->opcode, "SUB") == 0) {

    	stage->buffer = cpu->stage[EX1].rd;
    }

    if (strcmp(stage->opcode, "SUBL") == 0) {

    	stage->buffer = cpu->stage[EX1].rd;
    }

    if (strcmp(stage->opcode, "MUL") == 0) {

    	stage->buffer = cpu->stage[EX1].rd;
    }

    if (strcmp(stage->opcode, "OR") == 0) {
      
      	stage->buffer = cpu->stage[EX1].rd;
    }

    if (strcmp(stage->opcode, "EX-OR") == 0) {
      
      	stage->buffer = cpu->stage[EX1].rd;
    }

    if (strcmp(stage->opcode, "AND") == 0) {
      
      stage->buffer = cpu->stage[EX1].rd;
    }

    if (strcmp(stage->opcode, "BZ") == 0) {
      
      
    }*/
      if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Execute2", stage);
    }
    cpu->stage[MEM1] = cpu->stage[EX2];

    
  } else{

  	if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Execute2", stage);
    }

    cpu->stage[MEM1] = cpu->stage[EX2];
  }
  return 0;
}

/*
 *  Memory Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
memory1(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[MEM1];

  stageMEM1 = stage->pc;

  if (!stage->busy && !stage->stalled) {

  	if (strcmp(stage->opcode, "HALT") == 0) { 	
     
    }

    /* Store */
    if (strcmp(stage->opcode, "STORE") == 0) {

    	cpu->data_memory[stage->mem_address] = stage->rs1_value;
    }

    if (strcmp(stage->opcode, "STR") == 0) {

    	cpu->data_memory[stage->mem_address] = cpu->regs[stage->rd];
    }

    if (strcmp(stage->opcode, "LOAD") == 0) {

      stage->buffer = cpu->data_memory[stage->mem_address];
    }

    if (strcmp(stage->opcode, "LDR") == 0) {

      stage->buffer = cpu->data_memory[stage->mem_address];
    }

    /* MOVC */
    if (strcmp(stage->opcode, "MOVC") == 0) {
    }

    if (strcmp(stage->opcode, "BZ") == 0) {

      if(BZ_Flag == 0){
                cpu->pc = stage->buffer;
                
                make_register_valid(cpu, &cpu->stage[EX2]);
                make_register_valid(cpu, &cpu->stage[EX1]);
                make_register_valid(cpu, &cpu->stage[DRF]);
                make_register_valid(cpu, &cpu->stage[F]);
                make_stage_empty(&cpu->stage[DRF]); 
                make_stage_empty(&cpu->stage[EX1]);
                make_stage_empty(&cpu->stage[EX2]);
      }else{
                cpu->pc = stage->buffer;             
      }
      
    }

    if (strcmp(stage->opcode, "BNZ") == 0) {

      if(BZ_Flag == 1){
                cpu->pc = stage->buffer;
                
                make_register_valid(cpu, &cpu->stage[EX2]);
                make_register_valid(cpu, &cpu->stage[EX1]);
                make_register_valid(cpu, &cpu->stage[DRF]);
                make_register_valid(cpu, &cpu->stage[F]);
                make_stage_empty(&cpu->stage[DRF]); 
                make_stage_empty(&cpu->stage[EX1]);
                make_stage_empty(&cpu->stage[EX2]);
      }else{
                cpu->pc = stage->buffer;             
      }
      
    }

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Memory1", stage);
    }

    cpu->stage[MEM2] = cpu->stage[MEM1];
  } else{
  	
  	if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Memory1", stage);
    }

    cpu->stage[MEM2] = cpu->stage[MEM1];
  }

  return 0;
}


/*
 *  Memory Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
memory2(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[MEM2];

  stageMEM2 = stage->pc;

  if (!stage->busy && !stage->stalled) {

  	if (strcmp(stage->opcode, "HALT") == 0) {
	   
	  	
	  	
     
    }

    /* Store */
    if (strcmp(stage->opcode, "STORE") == 0) {
    }

    if (strcmp(stage->opcode, "LOAD") == 0) {

    	stage->buffer = cpu->stage[MEM1].rd;
    }

    if (strcmp(stage->opcode, "LDR") == 0) {

    	stage->buffer = cpu->stage[MEM1].rd;
    }

    /* MOVC */
    if (strcmp(stage->opcode, "MOVC") == 0) {
    }

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Memory2", stage);
    }

    cpu->stage[WB] = cpu->stage[MEM2];
  } else{

make_stage_empty(stage);
    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Memory2", stage);
    }

    cpu->stage[WB] = cpu->stage[MEM2];
  }

  return 0;
}


/*
 *  Writeback Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
writeback(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[WB];
  	

  if (!stage->busy && !stage->stalled) {

  	if (strcmp(stage->opcode, "HALT") == 0) { }
  	

    /* Update register file */
    if (strcmp(stage->opcode, "MOVC") == 0) {
      cpu->regs[stage->rd] = stage->buffer;
      cpu->regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;

    }

    if (strcmp(stage->opcode, "LOAD") == 0) {
      cpu->regs[stage->rd] = stage->buffer;
      cpu->regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;
    }

    if (strcmp(stage->opcode, "ADD") == 0) {

      if (!stage->buffer) {
                BZ_Flag = 0;
           } else {

                BZ_Flag = 1;
              }
      cpu->regs[stage->rd] = stage->buffer;
      cpu->regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;

    }

    if (strcmp(stage->opcode, "SUB") == 0) {

      if (!stage->buffer) {
                BZ_Flag = 0;
           } else {

                BZ_Flag = 1;
              }
      cpu->regs[stage->rd] = stage->buffer;
      cpu->regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;
    }

    if (strcmp(stage->opcode, "OR") == 0) {
      cpu->regs[stage->rd] = stage->buffer;
      cpu->regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;
    }

    if (strcmp(stage->opcode, "AND") == 0) {
      cpu->regs[stage->rd] = stage->buffer;
      cpu->regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;
    }

    if (strcmp(stage->opcode, "EX-OR") == 0) {
      cpu->regs[stage->rd] = stage->buffer;
      cpu->regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;
    }

    if (strcmp(stage->opcode, "MUL") == 0) {
      if (!stage->buffer) {
                BZ_Flag = 0;
                
           } else {

                BZ_Flag = 1;
                
              }
      cpu->regs[stage->rd] = stage->buffer;
      cpu->regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;
    }

    cpu->ins_completed++;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Writeback", stage);
    }
  } else{

make_stage_empty(stage);
if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Writeback", stage);
    }  	
  }


  return 0;
}

/*
 *  APEX CPU simulation loop
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
APEX_cpu_run(APEX_CPU* cpu, char * argv[], int n)
{
	if(strcmp(argv[2],"display") == 0){

		while (cpu->clock < n) {

    		if (ENABLE_DEBUG_MESSAGES) {
      			printf("--------------------------------\n");
      			printf("Clock Cycle #: %d\n", cpu->clock + 1);
      			printf("--------------------------------\n");
    		}

    		writeback(cpu);
    		memory2(cpu);
    		memory1(cpu);
    		execute2(cpu);
    		execute1(cpu);
    		decode(cpu);
    		fetch(cpu);
    		cpu->clock++;
  		}

			printf("============= Register File =============\n");
  			printf("1 -> Valid Register\n0-> Invalid Register\n");
  				for(int i=0;i<16;i++){
  					printf("| Reg[%d] |  Values = %d |\n",i,cpu->regs[i]);
				}

  			printf("====== State of Data Memory ======\n");
  				for(int i=0;i<50;i++){
  					printf("| MEM[%d] | Data Value = %d |\n",i,cpu->data_memory[i]);
				}
		

		return 0;
 	}

	if(strcmp(argv[2],"simulate") == 0){

		while (cpu->clock < n) {

    		if (ENABLE_DEBUG_MESSAGES) {
      			printf("--------------------------------\n");
      			printf("Clock Cycle #: %d\n", cpu->clock + 1);
      			printf("--------------------------------\n");
    		}

    		writeback(cpu);
    		memory2(cpu);
    		memory1(cpu);
    		execute2(cpu);
    		execute1(cpu);
    		decode(cpu);
    		fetch(cpu);
    		cpu->clock++;
  		}

			printf("============= Register File =============\n");
  			printf("1 -> Valid Register\n0-> Invalid Register\n");
  				for(int i=0;i<16;i++){
  					printf("| Reg[%d] |  Values = %d |\n",i,cpu->regs[i]);
				}

  			printf("====== State of Data Memory ======\n");
  				for(int i=0;i<50;i++){
  					printf("| MEM[%d] | Data Value = %d |\n",i,cpu->data_memory[i]);
				}
		

		return 0;
	}
}
