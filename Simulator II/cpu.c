/*
 *  cpu.c
 *  Contains APEX cpu pipeline implementation
 *
 *  Author :
 *  Gaurav Kothari (gkothar1@binghamton.edu)
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
int count = 0;
int physical_register_count = 0;
int issue_count = 0;
int ARF[24];
int PRF[24];


/*
 * This function creates and initializes APEX cpu.
 *
 * Note : You are free to edit this function according to your
 * 				implementation
 */
APEX_CPU* APEX_cpu_init(const char* filename)
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
  memset(cpu->regs, 0, sizeof(int) * 24);
  memset(cpu->regs_valid, 1, sizeof(int) * 24);
  memset(cpu->phy_regs, 0, sizeof(int) * 24);
  memset(cpu->phy_regs_valid, 1, sizeof(int) * 24);
  memset(cpu->stage, 0, sizeof(CPU_Stage) * NUM_STAGES);
  memset(cpu->data_memory, 0, sizeof(int) * 4000);

  for(int i = 0; i<24; i++){

    ARF[i] = 100;
  }

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

      printf("%-9s %-9d %-9d %-9d %-9d\n",
             cpu->code_memory[i].opcode,
             cpu->code_memory[i].rd,
             cpu->code_memory[i].rs1,
             cpu->code_memory[i].rs2,
             cpu->code_memory[i].imm);
    }
  }

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

static void
print_renamed_instruction(CPU_Stage* stage)
{
  if (strcmp(stage->opcode, "STORE") == 0) {
    printf(
      "%s,P%d,P%d,#%d ", stage->opcode, stage->rs1, stage->rs2, stage->imm);
  }

  if (strcmp(stage->opcode, "STR") == 0) {
    printf(
      "%s,P%d,P%d,P%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "LOAD") == 0) {
    printf(
      "%s,P%d,P%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
  }

  if (strcmp(stage->opcode, "LDR") == 0) {
    printf(
      "%s,P%d,P%d,P%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "MOVC") == 0) {
    printf("%s,P%d,#%d ", stage->opcode, stage->rd, stage->imm);
  }

  if (strcmp(stage->opcode, "ADD") == 0) {
    printf(
      "%s,P%d,P%d,P%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "ADDL") == 0) {
    printf(
      "%s,P%d,P%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
  }

  if (strcmp(stage->opcode, "SUB") == 0) {
    printf(
      "%s,P%d,P%d,P%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "SUBL") == 0) {
    printf(
      "%s,P%d,P%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
  }

  if (strcmp(stage->opcode, "MUL") == 0) {
    printf(
      "%s,P%d,P%d,P%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "BZ") == 0) {
    printf(
      "%s,#%d ", stage->opcode, stage->imm);
  }

  if (strcmp(stage->opcode, "AND") == 0) {
    printf("%s,P%d,P%d,P%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "OR") == 0) {
    printf("%s,P%d,P%d,P%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "EX-OR") == 0) {
    printf("%s,P%d,P%d,P%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "HALT") == 0){
    printf("HALT");
  }

  if (strcmp(stage->opcode, "JUMP") == 0) {
    printf("%s,P%d,#%d", stage->opcode,stage->rs1,stage->imm);
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

static void
print_renamed_stage_content(char* name, CPU_Stage* stage)
{
  printf("%-15s: pc(%d) ", name, stage->pc);
  print_renamed_instruction(stage);
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

int
decode(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[DRF];



  if (!stage->busy && !stage->stalled) {

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Pre Renaming Ins", stage);
    }

    if (strcmp(stage->opcode, "MOVC") == 0) {

      if(ARF[stage->rd] == 100){

          ARF[stage->rd] = physical_register_count;
          PRF[physical_register_count] = stage->rd;
          stage->rd = physical_register_count;
          physical_register_count++;
        } else{

          stage->rd = ARF[stage->rd];
        }
    }

    /* STORE */
    if(strcmp(stage->opcode, "STR") == 0|| strcmp(stage->opcode, "LDR") == 0) {

        if(ARF[stage->rd] == 100){

          ARF[stage->rd] = physical_register_count;
          PRF[physical_register_count] = stage->rd;
          stage->rd = physical_register_count;
          physical_register_count++;
        } else{

          stage->rd = ARF[stage->rd];
        }

      if(ARF[stage->rs1] == 100){

          ARF[stage->rs1] = physical_register_count;
          PRF[physical_register_count] = stage->rs1;
          stage->rs1 = physical_register_count;
          physical_register_count++;
        }else{

          stage->rs1 = ARF[stage->rs1];
        }

      if(ARF[stage->rs2] == 100){

          ARF[stage->rs2] = physical_register_count;
          PRF[physical_register_count] = stage->rs2;
          stage->rs2 = physical_register_count;
          physical_register_count++;
        }else{

          stage->rs2 = ARF[stage->rs2];
        }
        cpu->stage[LSQ] = cpu->stage[DRF];
    }else{
      make_stage_empty(&cpu->stage[LSQ]);
    }

    if(strcmp(stage->opcode, "STORE") == 0 || strcmp(stage->opcode, "LOAD") == 0) {

        if(ARF[stage->rd] == 100){

          ARF[stage->rd] = physical_register_count;
          PRF[physical_register_count] = stage->rd;
          stage->rd = physical_register_count;
          physical_register_count++;
        } else{

          stage->rd = ARF[stage->rd];
        }

      if(ARF[stage->rs1] == 100){

          ARF[stage->rs1] = physical_register_count;
          PRF[physical_register_count] = stage->rs1;
          stage->rs1 = physical_register_count;
          physical_register_count++;
        }else{

          stage->rs1 = ARF[stage->rs1];
        }
        cpu->stage[LSQ] = cpu->stage[DRF];
    }else{
      make_stage_empty(&cpu->stage[LSQ]);
    }

    if (strcmp(stage->opcode, "ADD") == 0 || strcmp(stage->opcode, "MUL") == 0 || strcmp(stage->opcode, "SUB") == 0 || strcmp(stage->opcode, "OR") == 0 || strcmp(stage->opcode, "EX-OR") == 0 || strcmp(stage->opcode, "AND") == 0) {

        if(ARF[stage->rd] == 100){

          ARF[stage->rd] = physical_register_count;
          PRF[physical_register_count] = stage->rd;
          stage->rd = physical_register_count;
          physical_register_count++;
        } else{

          stage->rd = ARF[stage->rd];
        }

      if(ARF[stage->rs1] == 100){

          ARF[stage->rs1] = physical_register_count;
          PRF[physical_register_count] = stage->rs1;
          stage->rs1 = physical_register_count;
          physical_register_count++;
        }else{

          stage->rs1 = ARF[stage->rs1];
        }

      if(ARF[stage->rs2] == 100){

          ARF[stage->rs2] = physical_register_count;
          PRF[physical_register_count] = stage->rs2;
          stage->rs2 = physical_register_count;
          physical_register_count++;
        }else{

          stage->rs2 = ARF[stage->rs2];
        }
    }

    if (strcmp(stage->opcode, "ADDL") == 0 || strcmp(stage->opcode, "SUBL") == 0) {

      if(ARF[stage->rd] == 100){

          ARF[stage->rd] = physical_register_count;
          PRF[physical_register_count] = stage->rd;
          stage->rd = physical_register_count;
          physical_register_count++;
        } else{

          stage->rd = ARF[stage->rd];
        }

      if(ARF[stage->rs1] == 100){

          ARF[stage->rs1] = physical_register_count;
          PRF[physical_register_count] = stage->rs1;
          stage->rs1 = physical_register_count;
          physical_register_count++;
        }else{

          stage->rs1 = ARF[stage->rs1];
        }
    }

    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("Decode/RF", stage);
    }

    cpu->stage[IQ] = cpu->stage[DRF];
    cpu->reorder_buffer[count] = cpu->stage[DRF];
    if(cpu->reorder_buffer[count].pc > 0){
      count++;
    }


    
} else{
	if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("Decode/RF", stage);
    }
}

 
  return 0;
}

int
reorderBuffer(APEX_CPU* cpu)
{
    int i = 0;
    while(i != count){

      if (ENABLE_DEBUG_MESSAGES && cpu->reorder_buffer[i].pc != 0) {
      
      print_renamed_stage_content("ROB", &cpu->reorder_buffer[i]);
    }
    i++;

    }

  return 0;
}

int
issueQueue(APEX_CPU* cpu)
{ 
    cpu->issue_queue[issue_count] = cpu->stage[IQ];
    CPU_Stage* stage = &cpu->issue_queue[issue_count];

  if (!stage->busy && !stage->stalled) {

    if (strcmp(stage->opcode, "STORE") == 0 || strcmp(stage->opcode, "STR") == 0 || strcmp(stage->opcode, "LOAD") == 0 || strcmp(stage->opcode, "LDR") == 0) {

      if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("IQ", stage);
      make_stage_empty(&cpu->stage[MUL1]);
      make_stage_empty(&cpu->stage[BP_FU]);
      make_stage_empty(&cpu->stage[INT1]);

    }
    return 0;
    }

    if (strcmp(stage->opcode, "MOVC") == 0 || strcmp(stage->opcode, "ADD") == 0 || strcmp(stage->opcode, "ADDL") == 0 || strcmp(stage->opcode, "SUB") == 0 || strcmp(stage->opcode, "SUBL") == 0 || strcmp(stage->opcode, "OR") == 0 || strcmp(stage->opcode, "EX-OR") == 0 || strcmp(stage->opcode, "AND") == 0) {

      if (strcmp(stage->opcode, "MOVC") == 0) {

      if(cpu->phy_regs_valid[stage->rd] == 0){
      stage->stalled = 1;
      } else{
      stage->stalled = 0;
      cpu->phy_regs_valid[stage->rd] = 0;
      }
    }

    if (strcmp(stage->opcode, "ADD") == 0) {

      if(cpu->phy_regs_valid[stage->rs1] == 0 || cpu->phy_regs_valid[stage->rs2] == 0){
        
        stage->stalled = 1;
      } else{

        stage->rs1_value = cpu->phy_regs[stage->rs1];
        stage->rs2_value = cpu->phy_regs[stage->rs2];
        cpu->phy_regs_valid[stage->rd] = 0;
      }
      
    }

    if (strcmp(stage->opcode, "AND") == 0) {

      if(cpu->phy_regs_valid[stage->rs1] == 0 || cpu->phy_regs_valid[stage->rs2] == 0){
      stage->stalled = 1;
      } else{
      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
        stage->rs2_value = cpu->phy_regs[stage->rs2];
        cpu->phy_regs_valid[stage->rd] = 0;
      }
      
    }

    if (strcmp(stage->opcode, "SUB") == 0) {

      if(cpu->phy_regs_valid[stage->rs1] == 0 || cpu->phy_regs_valid[stage->rs2] == 0){
      stage->stalled = 1;
      } else{
      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
        stage->rs2_value = cpu->phy_regs[stage->rs2];
        cpu->phy_regs_valid[stage->rd] = 0;
      }
    }

    if (strcmp(stage->opcode, "ADDL") == 0) {

      if(cpu->phy_regs_valid[stage->rd] == 0 || cpu->phy_regs_valid[stage->rs1] == 0){
      //stage->stalled = 1;
      } else{
      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
        cpu->phy_regs_valid[stage->rd] = 0;
      }
    }

    if (strcmp(stage->opcode, "SUBL") == 0) {

      if(cpu->phy_regs_valid[stage->rd] == 0 || cpu->phy_regs_valid[stage->rs1] == 0){
      stage->stalled = 1;
      } else{
      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
        cpu->phy_regs_valid[stage->rd] = 0;
      }
    }

    if (strcmp(stage->opcode, "MUL") == 0) {

      if(cpu->phy_regs_valid[stage->rs1] == 0 || cpu->phy_regs_valid[stage->rs2] == 0){
      stage->stalled = 1;
      } else{
      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
        stage->rs2_value = cpu->phy_regs[stage->rs2];
        cpu->phy_regs_valid[stage->rd] = 0;
      }
    }
  
  /* Read data from register file for Or */
    if (strcmp(stage->opcode, "OR") == 0) {
     if(cpu->phy_regs_valid[stage->rs1] == 0 || cpu->phy_regs_valid[stage->rs2] == 0){
      stage->stalled = 1;
      } else{
      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
        stage->rs2_value = cpu->phy_regs[stage->rs2];
        cpu->phy_regs_valid[stage->rd] = 0;
      }
    }
  

    /* Read data from register file for Ex-Or */
    if (strcmp(stage->opcode, "EX-OR") == 0) {
     if(cpu->phy_regs_valid[stage->rs1] == 0 || cpu->phy_regs_valid[stage->rs2] == 0){

      stage->stalled = 1;
      } else{

      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
          stage->rs2_value = cpu->phy_regs[stage->rs2];
          cpu->phy_regs_valid[stage->rd] = 0;
      }
     
    }

      if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("IQ", stage);
      make_stage_empty(&cpu->stage[MUL1]);
      make_stage_empty(&cpu->stage[BP_FU]);
      make_stage_empty(&cpu->stage[MEM_FU]);

    }
    cpu->stage[INT1] = cpu->stage[IQ];
    return 0;
    }

    if (strcmp(stage->opcode, "MUL") == 0) {

      if(cpu->phy_regs_valid[stage->rs1] == 0 || cpu->phy_regs_valid[stage->rs2] == 0){
      stage->stalled = 1;
      } else{
      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
        stage->rs2_value = cpu->phy_regs[stage->rs2];
        cpu->phy_regs_valid[stage->rd] = 0;
      }

      if (ENABLE_DEBUG_MESSAGES) {

            print_renamed_stage_content("IQ", stage);
            make_stage_empty(&cpu->stage[BP_FU]);
            make_stage_empty(&cpu->stage[INT1]);
            make_stage_empty(&cpu->stage[MEM_FU]);
        }

        cpu->stage[MUL1] = cpu->stage[IQ];
        return 0;

    }

    if (strcmp(stage->opcode, "BZ") == 0 || strcmp(stage->opcode, "JUMP") == 0 || strcmp(stage->opcode, "BNZ") == 0) {

        if (ENABLE_DEBUG_MESSAGES) {

            print_renamed_stage_content("IQ", stage);
            make_stage_empty(&cpu->stage[MUL1]);
            make_stage_empty(&cpu->stage[INT1]);
            make_stage_empty(&cpu->stage[MEM_FU]);
        }
        cpu->stage[BP_FU] = cpu->stage[IQ];
        
        return 0;
    }       

    int iq = 0;
    while(iq != count){

      if (ENABLE_DEBUG_MESSAGES && cpu->issue_queue[iq].pc != 0) {
      
      print_renamed_stage_content("IQ", &cpu->issue_queue[iq]);
    }
    iq++;

    }     
}

else{

  issue_count++;

  if (strcmp(cpu->stage[DRF].opcode, "BZ") == 0) {

      cpu->stage[DRF].stalled = 0;
    }

  make_stage_empty(stage);

    int iq = 0;
    while(iq != count){

      if (ENABLE_DEBUG_MESSAGES && cpu->issue_queue[iq].pc != 0) {
      
      print_renamed_stage_content("IQ", &cpu->issue_queue[iq]);
    }
    iq++;

    }
    cpu->stage[INT1] = cpu->stage[IQ];
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
lsQueue(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[LSQ];

  if (!stage->busy && !stage->stalled) {

    if (strcmp(stage->opcode, "LOAD") == 0) {


     if(cpu->phy_regs_valid[stage->rs1] == 0 || cpu->phy_regs_valid[stage->rd] == 0){
      stage->stalled = 1;
      } else{
      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
        
        cpu->phy_regs_valid[stage->rd] = 0;
      }
    }

    /* Read data from register file for LDR */
    if (strcmp(stage->opcode, "LDR") == 0) {

      
     if(cpu->phy_regs_valid[stage->rs1] == 0 || cpu->phy_regs_valid[stage->rs2] == 0){
      stage->stalled = 1;
      } else{
      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
        stage->rs2_value = cpu->phy_regs[stage->rs2];
        cpu->phy_regs_valid[stage->rd] = 0;
      }
    }
  
  if (strcmp(stage->opcode, "STORE") == 0) {

      if(cpu->phy_regs_valid[stage->rs1] == 0){
      stage->stalled = 1;
      } else{
      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
      }
    }
  
  if (strcmp(stage->opcode, "STR") == 0) {

      if(cpu->phy_regs_valid[stage->rs1] == 0 || cpu->phy_regs_valid[stage->rs2] == 0){
      stage->stalled = 1;
      } else{
      stage->stalled = 0;
      stage->rs1_value = cpu->phy_regs[stage->rs1];
          stage->rs2_value = cpu->phy_regs[stage->rs2];
      }
    }

    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("LSQ", stage);
    }

    cpu->stage[MEM_FU] = cpu->stage[LSQ];
  } else{
  	
  	if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("LSQ", stage);
    }

    cpu->stage[MEM_FU] = cpu->stage[LSQ];
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
integer1(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[INT1];

  if (!stage->busy && !stage->stalled) {

    if (strcmp(stage->opcode, "HALT") == 0) {
     
      
      cpu->stage[F].stalled = 1;
      make_stage_empty(&cpu->stage[F]);
      make_stage_empty(&cpu->stage[DRF]);
      if (ENABLE_DEBUG_MESSAGES) {
          print_renamed_stage_content("INT1", stage);
      }

      cpu->stage[INT2] = cpu->stage[INT1];
      return 0;
     
    }

    /* MOVC */
    if(strcmp(stage->opcode, "MOVC") == 0) {

      stage->buffer = stage->imm;
    }

    if (strcmp(stage->opcode, "ADD") == 0) {

      stage->buffer = stage->rs1_value + stage->rs2_value;
      printf("ADDDDDDD stage->buffer : %d\n", stage->buffer);
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

    if (strcmp(stage->opcode, "OR") == 0) {

      stage->buffer = stage->rs1_value | stage->rs2_value;
    }

    if (strcmp(stage->opcode, "EX-OR") == 0) {

      stage->buffer = stage->rs1_value ^ stage->rs2_value;
    }

    if (strcmp(stage->opcode, "AND") == 0) {

      stage->buffer = stage->rs1_value&stage->rs2_value;
    }

    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("INT1", stage);
    }

    cpu->stage[INT2] = cpu->stage[INT1];
  } else{

    make_stage_empty(stage);
    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("INT1", stage);
    }

    cpu->stage[INT2] = cpu->stage[INT1];
  }

  return 0;
}

int
integer2(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[INT2];

  if (!stage->busy && !stage->stalled) {

    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("INT2", stage);
    }

    cpu->stage[RE_ROB] = cpu->stage[INT2];
  } else{

    make_stage_empty(stage);
    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("INT2", stage);
    }

    cpu->stage[RE_ROB] = cpu->stage[INT2];
  }

  return 0;
}

int
multi1(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[MUL1];

  if (!stage->busy && !stage->stalled) {



    if (strcmp(stage->opcode, "MUL") == 0) {

      stage->buffer = stage->rs1_value * stage->rs2_value;

    }

    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("MUL1", stage);
    }

    cpu->stage[MUL2] = cpu->stage[MUL1];
  } else{

    make_stage_empty(stage);
    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("MUL1", stage);
    }

    cpu->stage[MUL2] = cpu->stage[MUL1];
  }

  return 0;
}

int
multi2(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[MUL2];

  if (!stage->busy && !stage->stalled) {

    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("MUL2", stage);
    }

    cpu->stage[MUL3] = cpu->stage[MUL2];
  } else{

    make_stage_empty(stage);
    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("MUL2", stage);
    }

    cpu->stage[MUL3] = cpu->stage[MUL2];
  }

  return 0;
}

int
multi3(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[MUL3];

  if (!stage->busy && !stage->stalled) {

    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("MUL3", stage);
    }

    cpu->stage[RE_ROB] = cpu->stage[MUL3];
  } else{

    make_stage_empty(stage);
    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("MUL3", stage);
    }

    cpu->stage[RE_ROB] = cpu->stage[MUL3];
  }

  return 0;
}

int
branchFU(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[BP_FU];

  if (!stage->busy && !stage->stalled) {



    if (strcmp(stage->opcode, "BZ") == 0) {

        if (BZ_Flag == 0) {
              cpu->pc = stage->pc + stage->imm;             
                
        } else {

                cpu->pc = cpu->pc + 8;
        }
    }

    if (strcmp(stage->opcode, "JUMP") == 0) {

        cpu->pc = stage->pc + stage->imm;
    }
        

    if (strcmp(stage->opcode, "BNZ") == 0) {

      if (BZ_Flag == 1) {
              cpu->pc = stage->pc + stage->imm;             
                
        } else {
          
                cpu->pc = cpu->pc + 8;
        }
        
    }

    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("BP_FU", stage);
    }

    cpu->stage[RE_ROB] = cpu->stage[BP_FU];
  } else{

    make_stage_empty(stage);
    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("BP_FU", stage);
    }

    cpu->stage[RE_ROB] = cpu->stage[BP_FU];
  }

  return 0;
}

int
memoryFU(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[MEM_FU];

  if (!stage->busy && !stage->stalled) {

    /* STORE */
    if(strcmp(stage->opcode, "STORE") == 0) {
      stage->mem_address = stage->rs1_value + stage->imm;
    }

    if(strcmp(stage->opcode, "STR") == 0) {
      stage->mem_address = stage->rs1_value + stage->rs2_value;
    }

    if(strcmp(stage->opcode, "LOAD") == 0) {

      stage->mem_address = stage->rs1_value + stage->imm;
    }

    if(strcmp(stage->opcode, "LDR") == 0) {

      stage->mem_address = stage->rs1_value + stage->rs2_value;
    }

    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("MEM_FU", stage);
    }

    cpu->stage[RE_ROB] = cpu->stage[MEM_FU];
  } else{

    make_stage_empty(stage);
    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("MEM_FU", stage);
    }

    cpu->stage[RE_ROB] = cpu->stage[MEM_FU];
  }

  return 0;
}

int
retireROB(APEX_CPU* cpu)
{

    CPU_Stage* stage = &cpu->stage[RE_ROB];

    if (strcmp(stage->opcode, "MOVC") == 0) {
      cpu->phy_regs[stage->rd] = stage->buffer;
      cpu->phy_regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;

    }

    if (strcmp(stage->opcode, "LOAD") == 0 || strcmp(stage->opcode, "LDR") == 0 || strcmp(stage->opcode, "STORE") == 0 || strcmp(stage->opcode, "STR") == 0) {
      cpu->phy_regs[stage->rd] = stage->buffer;
      cpu->phy_regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;
    }

    if (strcmp(stage->opcode, "ADD") == 0 || strcmp(stage->opcode, "MUL") == 0 || strcmp(stage->opcode, "SUB") == 0) {

      if (!stage->buffer) {
                BZ_Flag = 0;
           } else {

                BZ_Flag = 1;
              }
      cpu->phy_regs[stage->rd] = stage->buffer;
      cpu->phy_regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;

    }

    if (strcmp(stage->opcode, "OR") == 0 || strcmp(stage->opcode, "AND") == 0 || strcmp(stage->opcode, "EX-OR") == 0) {
      cpu->phy_regs[stage->rd] = stage->buffer;
      cpu->phy_regs_valid[stage->rd] = 1;
      cpu->stage[DRF].stalled=0;
      stage->stalled= 0;
    }

    if (ENABLE_DEBUG_MESSAGES) {
      print_renamed_stage_content("RE_ROB", stage);
      for(int i = 0; i < stage->rd; i++){

        printf("ARF details: R%d : %d\n", PRF[i], cpu->phy_regs[i]); 
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
            printf("================================================================\n");
            printf("Clock Cycle #: %d\n", cpu->clock + 1);
            printf("================================================================\n");
        }

        retireROB(cpu);
        printf("-------------------------------\n");
        memoryFU(cpu);
        printf("-------------------------------\n");
        branchFU(cpu);
        printf("-------------------------------\n");
        multi3(cpu);
        multi2(cpu);
        multi1(cpu);
        printf("-------------------------------\n");
        integer2(cpu);
        integer1(cpu);
        printf("-------------------------------\n");
        reorderBuffer(cpu);
        printf("-------------------------------\n");
        lsQueue(cpu);
        printf("-------------------------------\n");
        issueQueue(cpu);
        printf("-------------------------------\n");
    		decode(cpu);
    		fetch(cpu);
    		cpu->clock++;
  		}

  			printf("====== State of Data Memory ======\n");
  				for(int i=0;i<25;i++){
  					printf("| MEM[%d] | Data Value = %d |\n",i,cpu->data_memory[i]);
				}
		

		return 0;
 	}

	if(strcmp(argv[2],"simulate") == 0){

		while (cpu->clock < n) {

    		if (ENABLE_DEBUG_MESSAGES) {
      			printf("================================================================\n");
      			printf("Clock Cycle #: %d\n", cpu->clock + 1);
      			printf("================================================================\n");
    		}

    		retireROB(cpu);
        printf("-------------------------------\n");
        memoryFU(cpu);
        printf("-------------------------------\n");
        branchFU(cpu);
        printf("-------------------------------\n");
        multi3(cpu);
        multi2(cpu);
        multi1(cpu);
        printf("-------------------------------\n");
        integer2(cpu);
        integer1(cpu);
        printf("-------------------------------\n");
        reorderBuffer(cpu);
        printf("-------------------------------\n");
        lsQueue(cpu);
        printf("-------------------------------\n");
        issueQueue(cpu);
        printf("-------------------------------\n");
        decode(cpu);
        fetch(cpu);
    		cpu->clock++;
  		}



  			printf("====== State of Data Memory ======\n");
  				for(int i=0;i<25;i++){
  					printf("| MEM[%d] | Data Value = %d |\n",i,cpu->data_memory[i]);
				}
		

		return 0;
	}
}