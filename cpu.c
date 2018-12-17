#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1

/*
 * This function creates and initializes APEX cpu.
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
  memset(cpu->regs, 0, sizeof(int) * 16);
  memset(cpu->regs_valid, 1, sizeof(int) * 16);
  memset(cpu->stage, 0, sizeof(CPU_Stage) * NUM_STAGES);
  memset(cpu->data_memory, 0, sizeof(int) * 4000);

  cpu->stage[EX].flush=0;

  for (int i =0; i<16; i++) {
    cpu->regs_valid[i] = 1;
  }
  //dispRegValid(cpu);

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

  /* Make all stages busy except Fetch stage, initally to start the pipeline */
  for (int i = 1; i < NUM_STAGES; ++i) {
    cpu->stage[i].busy = 1;
  }
  //dispRegValid(cpu);
  return cpu;
}

/*
 * This function de-allocates APEX cpu.
 *
 * Note : You are free to edit this function according to your
 *        implementation
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
  if (strcmp(stage->opcode, "STORE") == 0)
   {
    printf("%s,R%d,R%d,#%d ", stage->opcode, stage->rs1, stage->rs2, stage->imm);
   }

   if (strcmp(stage->opcode, "LOAD") == 0) 
  {
    printf("%s,R%d,R%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
  }

  if (strcmp(stage->opcode, "MOVC") == 0) 
  {
    printf("%s,R%d,#%d ", stage->opcode, stage->rd, stage->imm);
  }
  
  if (strcmp(stage->opcode, "ADD") == 0) 
  {
    printf("%s,R%d,R%d,R%d", stage->opcode, stage->rd, stage->rs1,stage->rs2);
  }
  
  if (strcmp(stage->opcode, "SUB") == 0) 
  {
    printf("%s,R%d,R%d,R%d", stage->opcode, stage->rd, stage->rs1,stage->rs2);
  }
  
  if (strcmp(stage->opcode, "AND") == 0) 
  {
    printf("%s,R%d,R%d,R%d", stage->opcode, stage->rd, stage->rs1,stage->rs2);
  }
  
  if (strcmp(stage->opcode, "OR") == 0) 
  {
    printf("%s,R%d,R%d,R%d", stage->opcode, stage->rd, stage->rs1,stage->rs2);
  }
  
  if (strcmp(stage->opcode, "XOR") == 0) 
  {
    printf("%s,R%d,R%d,R%d", stage->opcode, stage->rd, stage->rs1,stage->rs2);
  }
  
  if (strcmp(stage->opcode, "MUL") == 0) 
  {
    printf("%s,R%d,R%d,R%d", stage->opcode, stage->rd, stage->rs1,stage->rs2);
  }
  
  if (strcmp(stage->opcode, "BZ") == 0) 
  {
    printf("%s,#%d", stage->opcode, stage->imm);
  }
  
  if (strcmp(stage->opcode, "BNZ") == 0) 
  {
    printf("%s,#%d", stage->opcode, stage->imm);
  }
  
  if (strcmp(stage->opcode, "JUMP") == 0) 
  {
    printf("%s,R%d,#%d", stage->opcode,stage->rs1,stage->imm);
  }
  
  if (strcmp(stage->opcode, "HALT") == 0)
   {
    printf("%s", stage->opcode);
   }

   if (strcmp(stage->opcode, "") == 0)
   {
    printf("EMPTY");
   }
}

/* Debug function which dumps the cpu stage
 * content
 */
static void
print_stage_content(char* name, CPU_Stage* stage)
{
  printf("%-15s: pc(%d) ", name, stage->pc);
  print_instruction(stage);
  printf("\n");
}

/*
 *  Fetch Stage of APEX Pipeline implementation
 */
int
fetch(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[F];
  // printf("\nFBstage->regs[0]: %d\n", cpu->regs[0]);
  if(cpu->stage[EX].flush==1)
  {
     strcpy(cpu->stage[F].opcode, "");
     printf("Fetch         : EMPTY\n");    

  }
  
 else if (!stage->busy && !stage->stalled) 
  {  
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
    stage->rd = current_ins->rd;

// && strcmp(cpu->stage[DRF].opcode, "BZ") != 0 && strcmp(cpu->stage[DRF].opcode, "BNZ") != 0
  if(!cpu->stage[DRF].stalled)
  {
    /* Update PC for next instruction */
    cpu->pc += 4;

    /* Copy data from fetch latch to decode latch*/
    cpu->stage[DRF] = cpu->stage[F];
  }

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Fetch", stage);
    }
  }
  
  else if(stage->stalled==1 || stage->busy==1)    // To stall fetch when MUL enters Execute stage
  {
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
    stage->rd = current_ins->rd;
    
   if(ENABLE_DEBUG_MESSAGES)
    {
      print_stage_content("Fetch", stage);
    }
  }
  // printf("\nFAstage->regs[0]: %d\n", cpu->regs[0]);  
  return 0;
}

/*
 *  Decode Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 *         implementation
 */
int
decode(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[DRF];
  if(stage->stalled) {
    stage->stalled = 0;
  }
  // printf("\nDRFstage->regs[0]: %d\n", cpu->regs[0]);
  if(cpu->stage[EX].flush==1)
  {
     strcpy(cpu->stage[F].opcode, "");
     printf("Decode        : EMPTY\n");    

  }
    
  else if (!stage->busy && !stage->stalled) 
  {
    /* Read data from register file for store */
    // printf("\nEXstage->regs[0]: %d\n", cpu->regs[0]);
    if (strcmp(stage->opcode, "STORE") == 0) 
    {
      stage->arithmetic_instr = 0;
    //printf("\ncpu->regs_valid[%d] : %d && cpu->regs_valid[%d] : %d\n",stage->rs1, cpu->regs_valid[stage->rs1], stage->rs2, cpu->regs_valid[stage->rs2]);
    if(cpu->regs_valid[stage->rs1] && cpu->regs_valid[stage->rs2])  //Valid bits of regs checked for dependency
    {
      cpu->stage[F].stalled=0;
        cpu->stage[DRF].stalled=0;
        stage->rs1_value= cpu->regs[stage->rs1];
      stage->rs2_value= cpu->regs[stage->rs2];
    }
    else
    {
      cpu->stage[F].stalled=1; 
      cpu->stage[DRF].stalled=1;
    }
    }
  
  // LOAD
  if (strcmp(stage->opcode, "LOAD") == 0) 
  {
    stage->arithmetic_instr = 0;
    if(cpu->regs_valid[stage->rs1])       
    {
      cpu->stage[F].stalled=0; 
      cpu->stage[DRF].stalled=0;
      stage->rs1_value= cpu->regs[stage->rs1];  // 0 is invalid for dependency
    cpu->regs_valid[stage->rd]--; 
    }
    else
    {
      cpu->stage[F].stalled=1; 
      cpu->stage[DRF].stalled=1;
    }
          
    }

  /* Read data from register file for JUMP */
    if (strcmp(stage->opcode, "JUMP") == 0) 
    {
      stage->arithmetic_instr = 0;
    stage->rs1_value= cpu->regs[stage->rs1];
    }

    /* No Register file read needed for MOVC */
    if (strcmp(stage->opcode, "MOVC") == 0)
     {
      stage->arithmetic_instr = 0;    // 0 is invalid for dependency
    cpu->regs_valid[stage->rd]--;           
    }
  
  // ADD
  if (strcmp(stage->opcode, "ADD") == 0) 
  {
    stage->arithmetic_instr = 1;
    // printf("\ncpu->regs_valid[stage->rs1] %d: %d, cpu->regs_valid[stage->rs2] %d: %d ",stage->rs1, cpu->regs_valid[stage->rs1],stage->rs2, cpu->regs_valid[stage->rs2]);
    if(cpu->regs_valid[stage->rs1] && cpu->regs_valid[stage->rs2])  
    {
      cpu->stage[F].stalled=0; 
      cpu->stage[DRF].stalled=0;
      stage->rs1_value= cpu->regs[stage->rs1];
      stage->rs2_value= cpu->regs[stage->rs2];
      cpu->regs_valid[stage->rd]--;
    }
    else
    {
      cpu->stage[F].stalled=1; 
      cpu->stage[DRF].stalled=1;
    }
    
    }
  
  // SUB
  if (strcmp(stage->opcode, "SUB") == 0) 
  {
    stage->arithmetic_instr = 1;
    if(cpu->regs_valid[stage->rs1] && cpu->regs_valid[stage->rs2])  
    {
      cpu->stage[F].stalled=0; 
      cpu->stage[DRF].stalled=0;
      stage->rs1_value= cpu->regs[stage->rs1];
      stage->rs2_value= cpu->regs[stage->rs2];
      cpu->regs_valid[stage->rd]--;
    }
    else
    {
      cpu->stage[F].stalled=1; 
      cpu->stage[DRF].stalled=1;
    }
    }
  
  // AND
  if (strcmp(stage->opcode, "AND") == 0) 
  {
    stage->arithmetic_instr = 0;
    if(cpu->regs_valid[stage->rs1] && cpu->regs_valid[stage->rs2])  
    {
      cpu->stage[F].stalled=0; 
      cpu->stage[DRF].stalled=0;
      stage->rs1_value= cpu->regs[stage->rs1];
      stage->rs2_value= cpu->regs[stage->rs2];
      cpu->regs_valid[stage->rd]--;
    }
    else
    {
      cpu->stage[F].stalled=1; 
      cpu->stage[DRF].stalled=1;
    }  
    }
  
  // OR
  if (strcmp(stage->opcode, "OR") == 0) 
  {
    stage->arithmetic_instr = 0;
    if(cpu->regs_valid[stage->rs1] && cpu->regs_valid[stage->rs2])  
    {
      cpu->stage[F].stalled=0; 
      cpu->stage[DRF].stalled=0;
      stage->rs1_value= cpu->regs[stage->rs1];
      stage->rs2_value= cpu->regs[stage->rs2];
      cpu->regs_valid[stage->rd]--;
    }
    else
    {
      cpu->stage[F].stalled=1; 
      cpu->stage[DRF].stalled=1;
    } 
    }
  
  // XOR
  if (strcmp(stage->opcode, "XOR") == 0) 
  {
    stage->arithmetic_instr = 0;
    if(cpu->regs_valid[stage->rs1] && cpu->regs_valid[stage->rs2])  // For Dependency
    {
      cpu->stage[F].stalled=0; 
      cpu->stage[DRF].stalled=0;
      stage->rs1_value= cpu->regs[stage->rs1];
      stage->rs2_value= cpu->regs[stage->rs2];
      cpu->regs_valid[stage->rd]--;  
    }
    else
    {
      cpu->stage[F].stalled=1; 
      cpu->stage[DRF].stalled=1;
    }
      
    }
  
  // MUL
  if (strcmp(stage->opcode, "MUL") == 0) 
  {
    stage->arithmetic_instr = 1;
    // printf("\nDestination regs check for mul %d\n",cpu->regs_valid[stage->rd]);
    if(cpu->regs_valid[stage->rs1] && cpu->regs_valid[stage->rs2])  
    {
      cpu->stage[F].stalled=0; 
      cpu->stage[DRF].stalled=0;
      stage->rs1_value= cpu->regs[stage->rs1];
    stage->rs2_value= cpu->regs[stage->rs2];
    cpu->regs_valid[stage->rd]--;  
    // printf("Destination regs valid %d\n",cpu->regs_valid[stage->rd]);
    }
    else
    {
      // printf("Inside else for MUL stall\n");
      cpu->stage[F].stalled=1; 
      cpu->stage[DRF].stalled=1;
    }
    
    }

    /* HALT */
      if(strcmp(stage->opcode, "HALT") == 0) 
      {
        stage->arithmetic_instr = 0;
        cpu->stage[F].stalled = 1;
        cpu->stage[F].pc = 0;
        strcpy(cpu->stage[F].opcode, "");
        cpu->ex_halt = 1;
      }


      if(strcmp(stage->opcode, "BZ") == 0 || strcmp(stage->opcode, "BNZ") == 0) 
      {
      stage->arithmetic_instr = 0;
      if((cpu->stage[WB].arithmetic_instr == 1) || (cpu->stage[MEM].arithmetic_instr == 1)) 
      {
        stage->stalled = 1;
      } else {
        stage->stalled = 0;
      }
    }


    /* Copy data from decode latch to execute latch*/
    cpu->stage[EX] = cpu->stage[DRF];

    if(ENABLE_DEBUG_MESSAGES)
    {
      print_stage_content("Decode/RF", stage);
  }
  
  }
  
  else
   {  
   if(ENABLE_DEBUG_MESSAGES)
   {
      print_stage_content("Decode/RF", stage);
    }
  } 
   
  return 0;
}

/*
 *  Execute Stage of APEX Pipeline implementation
 */
int
execute(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[EX];
  // printf("\nEXstage->regs[0]: %d\n", cpu->regs[0]);
  if (!stage->busy && !stage->stalled) 
  {
    // printf("\nInside Execute IF\n");
    if (strcmp(stage->opcode, "STORE") == 0) 
    {
    stage->mem_address = stage->rs2_value + stage->imm;
    }

    if (strcmp(stage->opcode, "LOAD") == 0) 
  {
    stage->mem_address = stage->imm+stage->rs1_value;
    }

  if (strcmp(stage->opcode, "JUMP") == 0) 
  { 
    cpu->pc = stage->rs1_value + stage->imm;
  }
  
  //bz
  if (strcmp(stage->opcode, "BZ") == 0)
   {  
    // && stage->arithmetic_instr==1
    if(cpu->zero==1)
    {
      
    //cpu->pc =(stage->pc) + (stage->imm);
    //stage->mem_address = stage->pc + stage->imm;
      stage->mem_address = stage->pc + stage->imm;
      //cpu->pc = stage->mem_address;
      cpu->zero=0;
  } else {
    stage->mem_address = 0;
  }
  }
  
  if (strcmp(stage->opcode, "BNZ") == 0) 
  { 
    // && stage->arithmetic_instr==1
    if(!cpu->zero)
    {
    //cpu->pc =(stage->pc) + (stage->imm);
    stage->mem_address = stage->pc + stage->imm;
    //cpu->pc = stage->mem_address;
    //cpu->stage[MEM] = cpu->stage[EX];
    } else {
      stage->mem_address = 0;
    }
  }

    if (strcmp(stage->opcode, "MOVC") == 0) 
    {
      stage->buffer = stage->imm;
    }
  
  if (strcmp(stage->opcode, "ADD") == 0) 
  {
    stage->buffer = stage->rs1_value + stage->rs2_value;
    if(stage->buffer==0)
       cpu->zero=1;
     else
       cpu->zero=0;   
    }
  
  if (strcmp(stage->opcode, "SUB") == 0) 
  {
    stage->buffer = stage->rs1_value-stage->rs2_value;
    if(stage->buffer==0)
      cpu->zero=1;
    else
       cpu->zero=0; 
    }
  
  if (strcmp(stage->opcode, "AND") == 0) 
  {
    stage->buffer = stage->rs2_value&stage->rs1_value;
    }
  
  if (strcmp(stage->opcode, "OR") == 0) 
  {
    stage->buffer = stage->rs2_value | stage->rs1_value;
    }
  
  if (strcmp(stage->opcode, "XOR") == 0) 
  {
    stage->buffer = stage->rs2_value ^ stage->rs1_value;
    }

  if (strcmp(stage->opcode, "MUL") == 0) 
  {
    if(stage->mul_flag==0)
    {
      cpu->stage[F].stalled=1;
      cpu->stage[DRF].stalled=1;
      cpu->stage[F].busy=1;
      cpu->stage[DRF].busy=1;
      stage->nop=1;
    }
    
    if(stage->mul_flag==1)
    {
      stage->buffer = stage->rs1_value * stage->rs2_value;
      cpu->stage[F].stalled=0;
      cpu->stage[DRF].stalled=0;
      cpu->stage[F].busy=0;
      cpu->stage[DRF].busy=0;
      stage->nop=0;
    }
    stage->mul_flag=1;
    if(stage->buffer==0)
      cpu->zero=1;
    else
       cpu->zero=0;
    }

    if(strcmp(stage->opcode, "HALT") == 0)
     {
      stage->flush=1;
      cpu->stage[DRF].pc = 0;
       strcpy(cpu->stage[DRF].opcode, "");
       cpu->stage[DRF].stalled = 1;
      cpu->stage[F].stalled = 1;
       strcpy(cpu->stage[F].opcode, "");
      cpu->stage[F].pc = 0;
      cpu->ex_halt=1;
    }

    /* Copy data from Execute latch to Memory latch*/
    cpu->stage[MEM] = cpu->stage[EX];

    if(ENABLE_DEBUG_MESSAGES)
     {
      print_stage_content("Execute", stage);
     }
  }
  
  else
  {
   cpu->stage[MEM] = cpu->stage[EX];
  if(ENABLE_DEBUG_MESSAGES)
   {
      printf("Execute        : EMPTY\n");
    } 
  } 
  return 0;
}

/*
 *  Memory Stage of APEX Pipeline implementation
 */
int
memory(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[MEM];
  if (!stage->busy && !stage->stalled && stage->nop==0) 
  {
    // printf("\nInside Memory IF\n");
    /* Store */
    if (strcmp(stage->opcode, "STORE") == 0) 
    {
    cpu->data_memory[stage->mem_address] = stage->rs1_value;
    }
  
    /* LOAD */
    if (strcmp(stage->opcode, "LOAD") == 0) 
    {
    stage->buffer= cpu->data_memory[stage->mem_address];
    }

    if (strcmp(stage->opcode, "BZ") == 0) 
    {
      if(stage->mem_address != 0) {
        cpu->pc =stage->mem_address;


        //printf("\nTesting: %d: %d\n",cpu->stage[EX].rd,cpu->regs_valid[cpu->stage[EX].rd]);
        if((strcmp(cpu->stage[EX].opcode, "ADD") == 0) || (strcmp(cpu->stage[EX].opcode, "SUB") == 0) || (strcmp(cpu->stage[EX].opcode, "MUL") == 0) || (strcmp(cpu->stage[EX].opcode, "AND") == 0) || (strcmp(cpu->stage[EX].opcode, "OR") == 0) || (strcmp(cpu->stage[EX].opcode, "XOR") == 0) || (strcmp(cpu->stage[EX].opcode, "MOVC") == 0) || (strcmp(cpu->stage[EX].opcode, "LOAD") == 0)) {
          cpu->regs_valid[cpu->stage[EX].rd]++;
        }

          //stage->flush=1;
        cpu->stage[DRF].pc = 0;
         strcpy(cpu->stage[DRF].opcode, "");
         //cpu->stage[DRF].stalled = 1;
        //cpu->stage[EX].stalled = 1;
         strcpy(cpu->stage[EX].opcode, "");
        cpu->stage[EX].pc = 0;
        if(stage->imm < 0) {
          cpu->ins_completed = (cpu->ins_completed + (stage->imm/4))-1;
        }

        else{
          cpu->ins_completed = (cpu->ins_completed - (stage->imm/4));
        }
        if(cpu->ex_halt) {
          cpu->ex_halt = 0;
          cpu->stage[F].stalled =0;
        }
      }
    }
  
    if (strcmp(stage->opcode, "BNZ") == 0) 
    {
      if(stage->mem_address != 0) {
        cpu->pc =stage->mem_address;

      //printf("\nTesting: %d: %d\n",cpu->stage[EX].rd,cpu->regs_valid[cpu->stage[EX].rd]);
        if((strcmp(cpu->stage[EX].opcode, "ADD") == 0) || (strcmp(cpu->stage[EX].opcode, "SUB") == 0) || (strcmp(cpu->stage[EX].opcode, "MUL") == 0) || (strcmp(cpu->stage[EX].opcode, "AND") == 0) || (strcmp(cpu->stage[EX].opcode, "OR") == 0) || (strcmp(cpu->stage[EX].opcode, "XOR") == 0) || (strcmp(cpu->stage[EX].opcode, "MOVC") == 0) || (strcmp(cpu->stage[EX].opcode, "LOAD") == 0)) {
          cpu->regs_valid[cpu->stage[EX].rd]++;
        }

          //stage->flush=1;
        cpu->stage[DRF].pc = 0;
         strcpy(cpu->stage[DRF].opcode, "");
         //cpu->stage[DRF].stalled = 1;
        //cpu->stage[EX].stalled = 1;
         strcpy(cpu->stage[EX].opcode, "");
        cpu->stage[EX].pc = 0;

        if(stage->imm < 0) {
          cpu->ins_completed = (cpu->ins_completed + (stage->imm/4))-1;
        }

        else{
          cpu->ins_completed = (cpu->ins_completed - (stage->imm/4));
        }
        if(cpu->ex_halt) {
          cpu->ex_halt = 0;
          cpu->stage[F].stalled =0;
        }
      }
    }

  if(strcmp(stage->opcode, "HALT") == 0) 
  {
      cpu->stage[EX].pc = 0;
      strcpy(cpu->stage[EX].opcode, "");
      cpu->stage[DRF].pc = 0;
      strcpy(cpu->stage[DRF].opcode, "");
      cpu->stage[EX].stalled = 1;
      cpu->stage[DRF].stalled = 1;
       strcpy(cpu->stage[F].opcode, "");
      cpu->stage[F].stalled = 1;
      cpu->stage[F].pc = 0;
      cpu->ex_halt=1;
    }

    /* Copy data from decode latch to execute latch*/
    cpu->stage[WB] = cpu->stage[MEM];

    if (ENABLE_DEBUG_MESSAGES) 
    {
      print_stage_content("Memory", stage);
    }
  }
  
   else
   {
   cpu->stage[WB] = cpu->stage[MEM];
    if(ENABLE_DEBUG_MESSAGES)
    {
       printf("Memory         : EMPTY\n");
    } 
  }
  
  return 0;
}

/*
 *  Writeback Stage of APEX Pipeline implementation
 */
int
writeback(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[WB];
  // printf("\ncpu->regs_valid[stage->rd] %d:%d\n", stage->rd, cpu->regs_valid[stage->rd]);
  if (!stage->busy && !stage->stalled && stage->nop==0 && (strcmp(stage->opcode, "")!=0)) 
  {
    /* Update register file */
    if (strcmp(stage->opcode, "MOVC") == 0) 
  {
      cpu->regs[stage->rd] = stage->buffer;
    cpu->regs_valid[stage->rd]++;
    cpu->stage[DRF].stalled=0;
    cpu->stage[F].stalled=0;  
    }
  
  if (strcmp(stage->opcode, "LOAD") == 0) 
  {
      cpu->regs[stage->rd] = stage->buffer;
    cpu->regs_valid[stage->rd]++;
    cpu->stage[DRF].stalled=0;
    cpu->stage[F].stalled=0; 
    }
  
  
  if (strcmp(stage->opcode, "ADD") == 0) 
  {
      cpu->regs[stage->rd] = stage->buffer;
    cpu->regs_valid[stage->rd]++;
    cpu->stage[DRF].stalled=0;
    cpu->stage[F].stalled=0; 
    }
  
  if (strcmp(stage->opcode, "SUB") == 0) 
  {
      cpu->regs[stage->rd] = stage->buffer;
    cpu->regs_valid[stage->rd]++;
    cpu->stage[DRF].stalled=0;
    cpu->stage[F].stalled=0; 
    }
  
  if (strcmp(stage->opcode, "AND") == 0) 
  {
      cpu->regs[stage->rd] = stage->buffer;
    cpu->regs_valid[stage->rd]++;
    cpu->stage[DRF].stalled=0;
    cpu->stage[F].stalled=0; 
    }
  
  if (strcmp(stage->opcode, "OR") == 0) 
  {
      cpu->regs[stage->rd] = stage->buffer;
    cpu->regs_valid[stage->rd]++;
    cpu->stage[DRF].stalled=0;
    cpu->stage[F].stalled=0; 
    }
  
  if (strcmp(stage->opcode, "XOR") == 0) 
  {
      cpu->regs[stage->rd] = stage->buffer;
    cpu->regs_valid[stage->rd]++;
    cpu->stage[DRF].stalled=0;
    cpu->stage[F].stalled=0; 
    }
  
  if (strcmp(stage->opcode, "MUL") == 0) 
  {
      cpu->regs[stage->rd] = stage->buffer;
    cpu->regs_valid[stage->rd]++;
    cpu->stage[DRF].stalled=0;
    cpu->stage[F].stalled=0; 
    }

    if(strcmp(stage->opcode, "HALT") == 0) {
        cpu->ins_completed = cpu->code_memory_size - 1;
        cpu->stage[EX].pc = 0;
        strcpy(cpu->stage[EX].opcode, "");
        cpu->stage[DRF].pc = 0;
        strcpy(cpu->stage[DRF].opcode, "");
        cpu->stage[EX].stalled = 1;
        cpu->stage[DRF].stalled = 1;
        cpu->stage[F].stalled = 1;
         strcpy(cpu->stage[F].opcode, "");
        cpu->stage[F].pc = 0;
        cpu->stage[MEM].pc = 0;
        strcpy(cpu->stage[MEM].opcode, "");
        cpu->stage[MEM].stalled = 1;
        cpu->ex_halt=1;
      }
  
    cpu->ins_completed++;

    if (ENABLE_DEBUG_MESSAGES) 
    {
      print_stage_content("Writeback", stage);
    }
  }
  
   else
  {
   
  if(ENABLE_DEBUG_MESSAGES)
   {
      printf("Writeback      : EMPTY\n");

    } 
  }
 
  return 0;
}

void display(APEX_CPU* cpu)   // to display all register values
  {
    for(int i=0; i<16; i++)
    {
    printf("\nR%d = %d",i,cpu->regs[i]);
    }
  }
  
void valid(APEX_CPU* cpu)
  {
    for(int x=0;x<16;x++)
    {
      cpu->regs_valid[x]=2;
    }
  }
    

void validAll(APEX_CPU* cpu)
{
  for(int i=0;i<16;i++)
  {
    cpu->regs_valid[i]=1;
  }
}

void dispRegValue(APEX_CPU* cpu)
{
  for(int i=0;i<16;i++)
  {
    printf("\n\tR%d || Value = %d",i,cpu->regs[i]);
  }
}

/*void dispRegValid(APEX_CPU* cpu)
{
  for(int i=0;i<16;i++)
  {
    printf("\n\tR%d || Validity = %d",i,cpu->regs_valid[i]);
  }
}*/

void dispMem(APEX_CPU* cpu)
{
  for(int i=0;i<100;i++)
  {
    printf("\n\tMEM_Value[%d] || Value=%d",i,cpu->data_memory[i]);
  }
}

/*
 *  APEX CPU simulation loop
 */
int
APEX_cpu_run(APEX_CPU* cpu)
{
  //valid(cpu);
  
  while (1) 
  {

    /* All the instructions committed, so exit */
    if (cpu->ins_completed == cpu->code_memory_size || cpu->clock==cpu->no_cycles) 
    {
      printf("\n%d==%d || %d==%d\n",cpu->ins_completed, cpu->code_memory_size, cpu->clock, cpu->no_cycles);
      printf("(apex) >> Simulation Complete");
      break;
    }



    if(ENABLE_DEBUG_MESSAGES)
    {
      printf("--------------------------------\n");
      printf("Clock Cycle #: %d\n", cpu->clock);
      printf("--------------------------------\n");
    }
  
  // printf("\n\n\n============================REGISTER VALID=========================");
  // dispRegValid(cpu);
  
    writeback(cpu);
    memory(cpu);
    execute(cpu);
    decode(cpu);
    fetch(cpu);
    cpu->clock++;
}
  printf("\n");
  printf("=====REGISTER VALUE============\n");
  for(int i=0;i<16;i++)
  {printf("\n");
  printf(" | Register[%d] | Value=%d | status=%s | \n",i,cpu->regs[i],(cpu->regs_valid[i])?"Valid" : "Invalid");
      
  }
printf("=======DATA MEMORY===========\n");

  for(int i=0;i<99;i++)
  {
  printf(" | MEM[%d] | Value=%d | \n",i,cpu->data_memory[i]);
  }
  return 0;
  }