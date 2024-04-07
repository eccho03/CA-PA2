/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   proc.c                                                    */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <malloc.h>

#include "proc.h"
#include "mem.h"
#include "util.h"

/***************************************************************/
/* System (CPU and Memory) info.                                             */
/***************************************************************/
struct MIPS32_proc_t g_processor;


/***************************************************************/
/* Fetch an instruction indicated by PC                        */
/***************************************************************/
int fetch(uint32_t pc)
{
    return mem_read_32(pc); //reads a 32B data from the text memory.
}

/***************************************************************/
/* TODO: Decode the given encoded 32bit data (word)            */
/***************************************************************/
struct inst_t decode(int word)
{
    struct inst_t inst;
    //-----------------//
    if (!word) {
        g_processor.pc -= BYTES_PER_WORD;
        g_processor.running = FALSE;
    }
    OPCODE(inst) = (unsigned int) word >> 26; // opcode: 31~26 bit
    //printf("TEST opcode: %d", inst.opcode);
    if (OPCODE(inst) == 0) {
        // //R type - op code 0임
        // ** 32 bit 중 특정 비트 잘라내기 **//
        // word << n -> 상위 n bit 잘라냄
        // word >> (전체 비트 수 - (end_bit - start_bit + 1))
        RS(inst) = (unsigned int) (word << 6) >> (32 - (25 - 21 + 1));
        //printf("/ rs: %d", RS(inst));
        RT(inst) = (unsigned int) (word << 11) >> (32 - (20 - 16 + 1));
        //RT(inst) = (unsigned int) word >> 16; // rt: 20~16
        //printf("/ rt: %d", RT(inst));
        RD(inst) = (unsigned int) (word << 16) >> (32 - (15 - 11 + 1));
        //RD(inst) = (unsigned int) word >> 11; // rd: 15~11
        //printf("/ rd: %d", RD(inst));
        SHAMT(inst) = (unsigned int) (word << 21) >> (32 - (10 - 6 + 1));
        //SHAMT(inst) = (unsigned int) word >> 6; // shamt: 10~6
        //printf("/ shamt: %d", SHAMT(inst));
        FUNC(inst) = (unsigned int) (word << 26) >> (32 - (5 - 0 + 1));
        //FUNC(inst) = (unsigned int) word; // funct: 5~0
        //printf("/ func: %d\n", FUNC(inst));
        //---------------------------------------
    }

    else if (OPCODE(inst) == 2 || OPCODE(inst) == 3) {
        // J type
        TARGET(inst) = (word << 6) >> (32 - (25 - 0 + 1));
        //printf("/ target: %d\n", TARGET(inst));
    }

    else {
        //I type
        RS(inst) = (unsigned int) (word << 6) >> (32 - (25 - 21 + 1));
        //RS(inst) = (unsigned int) word >> 21; //rs: 25~21
        //printf("/ rs: %d", RS(inst));
        RT(inst) = (unsigned int) (word << 11) >> (32 - (20 - 16 + 1));
        //printf("/ rt: %d", RT(inst));
        IMM(inst) = (unsigned int)(word << 16) >> (32 - (15 - 0 + 1));
        //IMM(inst) = (unsigned int) word; //imm: 15~0
        //printf("/ imm: %d\n", IMM(inst));
    }

    //----------------//
    return inst;
}

/***************************************************************/
/* TODO: Execute the decoded instruction                       */
/***************************************************************/
void execute(struct inst_t inst)
{
    
    if (OPCODE(inst) == 0) {
        //R type
        if (FUNC(inst) == 0x20) {
            //add
            g_processor.regs[RD(inst)] = g_processor.regs[RS(inst)] + g_processor.regs[RT(inst)];
            //printf("[add] rd: %d\n", RD(inst)); 
        }

        else if (FUNC(inst) == 0x21) {
            //addu
            g_processor.regs[RD(inst)] = (unsigned int)g_processor.regs[RS(inst)] + (unsigned int)g_processor.regs[RT(inst)];
            //printf("[addu] rd: %d\n", RD(inst)); 
        }

        else if (FUNC(inst) == 0x24) {
            //and
            g_processor.regs[RD(inst)] = g_processor.regs[RS(inst)] & g_processor.regs[RT(inst)];
            //printf("[and] rd: %d\n", RD(inst)); 
        }

        else if (FUNC(inst) == 0x27) {
            //nor
            g_processor.regs[RD(inst)] = ~ (g_processor.regs[RS(inst)] | g_processor.regs[RT(inst)]);
            //printf("[nor] rd: %d\n", RD(inst)); 
        }

        else if (FUNC(inst) == 0x25) {
            //or
            g_processor.regs[RD(inst)] = g_processor.regs[RS(inst)] | g_processor.regs[RT(inst)];
            //printf("[or] rd: %d\n", RD(inst)); 
        }

        else if (FUNC(inst) == 0x2b) {
            //sltu
            g_processor.regs[RD(inst)] = (unsigned int)(g_processor.regs[RS(inst)] < g_processor.regs[RT(inst)]) ? 1:0;
        }

        else if (FUNC(inst) == 0x00) {
            //sll
            g_processor.regs[RD(inst)] = g_processor.regs[RT(inst)] << SHAMT(inst);
            //printf("[sll] rd: %d\n", RD(inst)); 
        }

        else if (FUNC(inst) == 0x02) {
            //srl
            g_processor.regs[RD(inst)] = g_processor.regs[RT(inst)] >> SHAMT(inst);
           //printf("[srl] rd: %d\n", RD(inst)); 
        }

        else if (FUNC(inst) == 0x22) {
            //sub
            g_processor.regs[RD(inst)] = g_processor.regs[RS(inst)] - g_processor.regs[RT(inst)];
            //printf("[sub] rd: %d\n", RD(inst)); 
        }

        else if (FUNC(inst) == 0x23) {
            //subu
            g_processor.regs[RD(inst)] = (unsigned)g_processor.regs[RS(inst)] - (unsigned)g_processor.regs[RT(inst)];
            //printf("[subu] rd: %d\n", RD(inst)); 
        }

        else if(FUNC(inst) == 0x08){ 
            //jr
            g_processor.pc = g_processor.regs[RS(inst)];
            //printf("[jr] rd: %d\n", RD(inst)); 
        }
    }
    //검토 완
    //---------------------------------------------------------------------------------------------------------------------------//
    //I type
    else if (OPCODE(inst) == 0x09) {
        //addiu
        g_processor.regs[RT(inst)] = (unsigned int)g_processor.regs[RS(inst)] + (unsigned int)IMM(inst);
        //printf("[addiu] rt: %d\n", RT(inst)); 
    }

    else if (OPCODE(inst) == 0x0c) {
        //andi
        g_processor.regs[RT(inst)] = g_processor.regs[RS(inst)] & IMM(inst);
        //printf("[andi] rt: %d\n", RT(inst));
    }

    else if (OPCODE(inst) == 0x04) {
        //beq
        if(g_processor.regs[RS(inst)] == g_processor.regs[RT(inst)]) {
            g_processor.pc = g_processor.pc + 4 * IMM(inst);
            //printf("[beq] pc: %d\n", g_processor.pc);
        }
    }

    else if (OPCODE(inst) == 0x05) {
        //bne
        if(g_processor.regs[RS(inst)] != g_processor.regs[RT(inst)]) {
            g_processor.pc = g_processor.pc + 4 * IMM(inst);
            //printf("[bne] pc: %d\n", g_processor.pc);
        }  
    }

    else if (OPCODE(inst) == 0x0f) {
        //lui
        g_processor.regs[RT(inst)] = IMM(inst) << 16;
        //printf("[lui] rt: %d\n", RT(inst));
    }

    else if (OPCODE(inst) == 0x23) {
        //lw 
        g_processor.regs[RT(inst)] = mem_read_32(g_processor.regs[RS(inst)] + IMM(inst));
        //printf("[lw] rt: %d\n", RT(inst));
    }

    else if (OPCODE(inst) == 0x0d) {
        //ori
        g_processor.regs[RT(inst)] = g_processor.regs[RS(inst)] | IMM(inst);
        //printf("[ori] rt: %d\n", RT(inst));
    }

    else if (OPCODE(inst) == 0x0b) {
        //sltiu
        g_processor.regs[RT(inst)] = (unsigned int)(g_processor.regs[RS(inst)] < IMM(inst)) ? 1 : 0;
        //printf("[sltu] rt: %d\n", RT(inst));
    }

    else if (OPCODE(inst) == 0x2b) {
        //sw
        mem_write_32(g_processor.regs[RS(inst)] + IMM(inst), g_processor.regs[RT(inst)]);
        //printf("[sw] rt: %d\n", RT(inst));
    }    
    //---------------------------------------------------------------------------------------------------------//
    //검토 완료
    //J type
    else if (OPCODE(inst) == 0x02) {
        //j
        //32bit 중에서 opcode 제외한 나머지 비트 떼어오기
        int pc_26bits = (g_processor.pc >> 28) << (32 - (31 - 28 + 1));
        TARGET(inst) = TARGET(inst) << 2;

        g_processor.pc = TARGET(inst) + pc_26bits;
    }

    else if (OPCODE(inst) == 0x03) {
        //jal
        g_processor.regs[31] = g_processor.pc + 4;
        int pc_26bits = (g_processor.pc >> 28) << (32 - (31 - 28 + 1));
        TARGET(inst) = TARGET(inst) << 2;
        
        g_processor.pc = TARGET(inst) + pc_26bits;
    }
    //------------------------------------------------------------//
}

/***************************************************************/
/* Advance a cycle                                             */
/***************************************************************/
void cycle()
{
    int inst_reg;
    struct inst_t inst;

    // 1. fetch
    inst_reg = fetch(g_processor.pc);
    g_processor.pc += BYTES_PER_WORD;

    // 2. decode
    inst = decode(inst_reg);

    // 3. execute
    execute(inst);

    // 4. update stats
    g_processor.num_insts++;
    g_processor.ticks++;
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump() {
    int k;

    printf("\n[INFO] Current register values :\n");
    printf("-------------------------------------\n");
    printf("PC: 0x%08x\n", g_processor.pc);
    printf("Registers:\n");
    for (k = 0; k < MIPS_REGS; k++)
        printf("R%d: 0x%08x\n", k, g_processor.regs[k]);
}



/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate MIPS for n cycles                      */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (g_processor.running == FALSE) {
        printf("[ERROR] Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("[INFO] Simulating for %d cycles...\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (g_processor.running == FALSE) {
            printf("[INFO] Simulator halted\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate MIPS until HALTed                      */
/*                                                             */
/***************************************************************/
void go() {
    if (g_processor.running == FALSE) {
        printf("[ERROR] Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("[INFO] Simulating...\n");
    while (g_processor.running)
        cycle();
    printf("[INFO] Simulator halted\n");
}
