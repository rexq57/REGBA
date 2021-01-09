#include "cpu.h"
#include "bus.h"

bool compile_arm(struct RECPU* cpu, uint32_t instruction);
bool disassemble(uint32_t instruction, uint32_t PC, char* buffer);

struct RECPU* recpu_create(void) {
    return (struct RECPU*)malloc(sizeof(struct RECPU));
}

void recpu_delete(struct RECPU* cpu) {
    free(cpu);
}

void recpu_init(struct RECPU* cpu, struct REBUS* bus) {
    
    // 清空寄存器
    memset(&cpu->regs, 0, sizeof(cpu->regs));
    memset(&cpu->_regs_IRQ, 0, sizeof(cpu->_regs_IRQ));
    memset(&cpu->_regs_FIQ, 0, sizeof(cpu->_regs_FIQ));
    memset(&cpu->_regs_SVC, 0, sizeof(cpu->_regs_SVC));
    memset(&cpu->_regs_ABT, 0, sizeof(cpu->_regs_ABT));
    memset(&cpu->_regs_UND, 0, sizeof(cpu->_regs_UND));
    memset(&cpu->cpsr, 0, sizeof(cpu->cpsr));
    
    // 初始化处理器各模式下的栈针，位于工作内存中
    memset(&cpu->mode_SP, 0, sizeof(cpu->mode_SP));
    cpu->mode_SP[PROCESSOR_MODE_USER] = 0x03007F00;
    cpu->mode_SP[PROCESSOR_MODE_SYS] = 0x03007F00;
    cpu->mode_SP[PROCESSOR_MODE_IRQ] = 0x03007FA0;
    cpu->mode_SP[PROCESSOR_MODE_SVC] = 0x03007FE0;
    
    // 处理器默认是用户模式
    recpu_set_mode(cpu, PROCESSOR_MODE_USER);
    
    cpu->bus = bus;
    
    cpu->current_cycle_count = 0;
};

void recpu_set_mode(struct RECPU* cpu, enum PROCESSOR_MODE mode) {
    
    // [注意] 在某些情况下，BIOS可能允许从SWI过程内部执行中断。 如果是这样，并且如果中断处理程序进一步调用SWI，则应注意Supervisor堆栈不会溢出。
    
    // 禁止从异常模式切换到异常模式
    REGBA_ASSERT(!(cpu->cpsr.mode > PROCESSOR_MODE_SYS && mode > PROCESSOR_MODE_SYS));
    
    // 直接拷贝数据到目标
#define CASE_BANK_REGS(x, name)  \
case PROCESSOR_MODE_##name: \
memcpy(&cpu->_regs_##name, &cpu->regs.r##x, sizeof(cpu->_regs_##name)); \
break;

#define CASE_RESTORE_REGS(x, name) \
case PROCESSOR_MODE_##name: \
memcpy(&cpu->regs.r##x, &cpu->_regs_##name, sizeof(cpu->_regs_##name)); \
break;
    
    // 模式相同时，不存只取，防止初始化时，SP未定义覆盖了有效值，后面再从中取出就造成错误
    if (mode != cpu->cpsr.mode) {
        // 存储当前模式的栈针
        if (cpu->cpsr.mode == PROCESSOR_MODE_USER || cpu->cpsr.mode == PROCESSOR_MODE_SYS) {
            cpu->mode_SP[PROCESSOR_MODE_USER] = cpu->regs.SP;
            cpu->mode_SP[PROCESSOR_MODE_SYS]  = cpu->regs.SP;
        } else {
            cpu->mode_SP[cpu->cpsr.mode] = cpu->regs.SP;
        }
    }
    
    // 设置切换模式的栈针，有些模式没有栈针，就是0，读写就会报错
    cpu->regs.SP = cpu->mode_SP[mode];
    
    // 寄存器保存/恢复
    switch (mode) {
        case PROCESSOR_MODE_USER:
        case PROCESSOR_MODE_SYS:
        {
            // 从上一个模式中恢复寄存器
            switch (cpu->cpsr.mode) {
                CASE_RESTORE_REGS(13, IRQ)
                CASE_RESTORE_REGS(8, FIQ)
                CASE_RESTORE_REGS(13, SVC)
                CASE_RESTORE_REGS(13, ABT)
                CASE_RESTORE_REGS(13, UND)
                default: break;
            }
            break;
        }
        // 进入特殊模式时，存储寄存器
        CASE_BANK_REGS(13, IRQ)
        CASE_BANK_REGS(8, FIQ)
        CASE_BANK_REGS(13, SVC)
        CASE_BANK_REGS(13, ABT)
        CASE_BANK_REGS(13, UND)
        default: break;
    }
    
    // 更改当前模式
    cpu->cpsr.mode = mode;
}

bool recpu_disassemble(struct REBUS* bus, uint32_t PC, char* buffer) {
    
    struct Data_OP op;
    rebus_mem_access(bus, PC, ACCESS_WIDTH_BIT_32, &op);
    uint32_t instruction = op.data;
    
    return disassemble(instruction, PC, buffer);
}

int recpu_run_next_instruction(struct RECPU* cpu, bool* error) {
    
    int cycle_count = 0;
    
    cpu->prev_PC = cpu->regs.PC;
    
    // 从内存总线上请求指令，并执行
    struct Data_OP op;
    rebus_mem_access(cpu->bus, cpu->regs.PC, ACCESS_WIDTH_BIT_32, &op);
    
    // 理论上指令读取不会出错
    REGBA_ASSERT(op.error == MEM_ERROR_NONE);
    
    uint32_t instruction = op.data;
    
    *error = !compile_arm(cpu, instruction);
    
    cpu->current_cycle_count += cycle_count;
    
    return cycle_count;
}

/* 指令条件检查
 
 Code Suffix Flags         Meaning
 0:   EQ     Z=1           equal (zero) (same)
 1:   NE     Z=0           not equal (nonzero) (not same)
 2:   CS/HS  C=1           unsigned higher or same (carry set)
 3:   CC/LO  C=0           unsigned lower (carry cleared)
 4:   MI     N=1           signed negative (minus)
 5:   PL     N=0           signed positive or zero (plus)
 6:   VS     V=1           signed overflow (V set)
 7:   VC     V=0           signed no overflow (V cleared)
 8:   HI     C=1 and Z=0   unsigned higher
 9:   LS     C=0 or Z=1    unsigned lower or same
 A:   GE     N=V           signed greater or equal
 B:   LT     N<>V          signed less than
 C:   GT     Z=0 and N=V   signed greater than
 D:   LE     Z=1 or N<>V   signed less or equal
 E:   AL     -             always (the "AL" suffix can be omitted)
 F:   NV     -             never (ARMv1,v2 only) (Reserved ARMv3 and up)

*/
static inline
bool conditionPassed(struct CPSR* cpsr, uint32_t instruction) {
    
    const int i = (instruction & 0xF0000000) >> 28;
    switch (i) {
        case 0x0: return cpsr->Z;     // EQ
        case 0x1: return !cpsr->Z;    // NE
        case 0x2: return cpsr->C;     // CS
        case 0x3: return !cpsr->C;    // CC
        case 0x4: return cpsr->N;     // MI
        case 0x5: return !cpsr->N;    // PL
        case 0x6: return cpsr->V;     // VS
        case 0x7: return !cpsr->V;    // VC
        case 0x8: return cpsr->C && !cpsr->Z; // HI
        case 0x9: return !cpsr->C || cpsr->Z; // LS
        case 0xA: return !cpsr->N == !cpsr->V; // GE
        case 0xB: return !cpsr->N != !cpsr->V; // LT
        case 0xC: return !cpsr->Z && !cpsr->N == !cpsr->V; // GT
        case 0xD: return cpsr->Z || !cpsr->N != !cpsr->V; // LE
        default:
            // AL
            // NV
            break;
    }
    return true;
}

struct Instruction_OP{
    bool writesPC, fixedJump;
};

/*
 ARM Binary Opcode Format
   |..3 ..................2 ..................1 ..................0|
   |1_0_9_8_7_6_5_4_3_2_1_0_9_8_7_6_5_4_3_2_1_0_9_8_7_6_5_4_3_2_1_0|
   |_Cond__|0_0_0_1_0_0_1_0_1_1_1_1_1_1_1_1_1_1_1_1|0_0|L|1|__Rn___| BX,BLX
   |_Cond__|0_0_0|___Op__|S|__Rn___|__Rd___|__Shift__|Typ|0|__Rm___| DataProc
   |_Cond__|0_0_0|___Op__|S|__Rn___|__Rd___|__Rs___|0|Typ|1|__Rm___| DataProc
   |_Cond__|0_0_1|___Op__|S|__Rn___|__Rd___|_Shift_|___Immediate___| DataProc
   |_Cond__|0_0_1_1_0_0_1_0_0_0_0_0_1_1_1_1_0_0_0_0|_____Hint______| ARM11:Hint
   |_Cond__|0_0_1_1_0|P|1|0|_Field_|__Rd___|_Shift_|___Immediate___| PSR Imm
   |_Cond__|0_0_0_1_0|P|L|0|_Field_|__Rd___|0_0_0_0|0_0_0_0|__Rm___| PSR Reg
 
   |1_1_1_0|0_0_0_1_0_0_1_0|_____immediate_________|0_1_1_1|_immed_| ARM9:BKPT
   |_Cond__|0_0_0_1_0_1_1_0_1_1_1_1|__Rd___|1_1_1_1|0_0_0_1|__Rm___| ARM9:CLZ
   |_Cond__|0_0_0_1_0|Op_|0|__Rn___|__Rd___|0_0_0_0|0_1_0_1|__Rm___| ARM9:QALU
   |_Cond__|0_0_0_0_0_0|A|S|__Rd___|__Rn___|__Rs___|1_0_0_1|__Rm___| Multiply
   |_Cond__|0_0_0_0_0_1_0_0|_RdHi__|_RdLo__|__Rs___|1_0_0_1|__Rm___| ARM11:UMAAL
   |_Cond__|0_0_0_0_1|U|A|S|_RdHi__|_RdLo__|__Rs___|1_0_0_1|__Rm___| MulLong
   |_Cond__|0_0_0_1_0|Op_|0|Rd/RdHi|Rn/RdLo|__Rs___|1|y|x|0|__Rm___| MulHalfARM9
   |_Cond__|0_0_0_1_0|B|0_0|__Rn___|__Rd___|0_0_0_0|1_0_0_1|__Rm___| TransSwp12
   |_Cond__|0_0_0_1_1|_Op__|__Rn___|__Rd___|1_1_1_1|1_0_0_1|__Rm___| ARM11:LDREX
   |_Cond__|0_0_0|P|U|0|W|L|__Rn___|__Rd___|0_0_0_0|1|S|H|1|__Rm___| TransReg10
   |_Cond__|0_0_0|P|U|1|W|L|__Rn___|__Rd___|OffsetH|1|S|H|1|OffsetL| TransImm10
   |_Cond__|0_1_0|P|U|B|W|L|__Rn___|__Rd___|_________Offset________| TransImm9
   |_Cond__|0_1_1|P|U|B|W|L|__Rn___|__Rd___|__Shift__|Typ|0|__Rm___| TransReg9
   |_Cond__|0_1_1|________________xxx____________________|1|__xxx__| Undefined
   |_Cond__|0_1_1|Op_|x_x_x_x_x_x_x_x_x_x_x_x_x_x_x_x_x_x|1|x_x_x_x| ARM11:Media
   |1_1_1_1_0_1_0_1_0_1_1_1_1_1_1_1_1_1_1_1_0_0_0_0_0_0_0_1_1_1_1_1| ARM11:CLREX
   |_Cond__|1_0_0|P|U|S|W|L|__Rn___|__________Register_List________| BlockTrans
   |_Cond__|1_0_1|L|___________________Offset______________________| B,BL,BLX
   |_Cond__|1_1_0|P|U|N|W|L|__Rn___|__CRd__|__CP#__|____Offset_____| CoDataTrans
   |_Cond__|1_1_0_0_0_1_0|L|__Rn___|__Rd___|__CP#__|_CPopc_|__CRm__| CoRR ARM9
   |_Cond__|1_1_1_0|_CPopc_|__CRn__|__CRd__|__CP#__|_CP__|0|__CRm__| CoDataOp
   |_Cond__|1_1_1_0|CPopc|L|__CRn__|__Rd___|__CP#__|_CP__|1|__CRm__| CoRegTrans
   |_Cond__|1_1_1_1|_____________Ignored_by_Processor______________| SWI
 */

bool compile_arm(struct RECPU* cpu, uint32_t instruction/*, struct Instruction_OP* op*/) {
    
    #define INSTRUCTION_ERROR REGBA_ASSERT(!"error");
    
    // 取25-27作为基本标志位
    uint32_t i = instruction & 0x0E000000;
    
    union REGS_USER* regs = &cpu->regs;
    
    // arm模式下，访问PC会返回提前两条指令的地址
    uint32_t PC = regs->PC + 8;
    
    // 先检查指令条件
    bool condOp = conditionPassed(&cpu->cpsr, instruction);
    if (!condOp) {
        return false;
    }
    
    // 根据标志位最长开始进行匹配判断
    if ((instruction & 0x0FFFFFF0) == 0x012FFF10) {
        /*
         Branch and Exchange (BX, BLX_reg)
           Bit    Expl.
           31-28  Condition
           27-8   Must be "0001.0010.1111.1111.1111" for this instruction
           7-4    Opcode
                   0001b: BX{cond}  Rn    ;PC=Rn, T=Rn.0   (ARMv4T and ARMv5 and up)
                   0010b: BXJ{cond} Rn    ;Change to Jazelle bytecode (ARMv5TEJ and up)
                   0011b: BLX{cond} Rn    ;PC=Rn, T=Rn.0, LR=PC+4     (ARMv5 and up)
           3-0    Rn - Operand Register  (R0-R14)
         Switching to THUMB Mode: Set Bit 0 of the value in Rn to 1, program continues then at Rn-1 in THUMB mode.
         Using BLX R14 is possible (sets PC=Old_LR, and New_LR=retadr).
         Using BX R15 acts as BX $+8 (tested and working on ARM7/ARM9, although it isn't officially defined as predictable behaviour).
         Execution Time: 2S + 1N
         Return: No flags affected.
         */
        INSTRUCTION_ERROR;
    } else if (!(instruction & 0x0C000000) && (i == 0x02000000 || (instruction & 0x00000090) != 0x00000090)) {
        INSTRUCTION_ERROR;
        /*
         ARM Opcodes: Data Processing (ALU)
         !(instruction & 0x0C000000): 0 0
         i == 0x02000000: Immediate
         Opcode Format
           Bit    Expl.
           31-28  Condition
           27-26  Must be 00b for this instruction
           25     I - Immediate 2nd Operand Flag (0=Register, 1=Immediate)
           24-21  Opcode (0-Fh)               ;*=Arithmetic, otherwise Logical
                    0: AND{cond}{S} Rd,Rn,Op2    ;AND logical       Rd = Rn AND Op2
                    1: EOR{cond}{S} Rd,Rn,Op2    ;XOR logical       Rd = Rn XOR Op2
                    2: SUB{cond}{S} Rd,Rn,Op2 ;* ;subtract          Rd = Rn-Op2
                    3: RSB{cond}{S} Rd,Rn,Op2 ;* ;subtract reversed Rd = Op2-Rn
                    4: ADD{cond}{S} Rd,Rn,Op2 ;* ;add               Rd = Rn+Op2
                    5: ADC{cond}{S} Rd,Rn,Op2 ;* ;add with carry    Rd = Rn+Op2+Cy
                    6: SBC{cond}{S} Rd,Rn,Op2 ;* ;sub with carry    Rd = Rn-Op2+Cy-1
                    7: RSC{cond}{S} Rd,Rn,Op2 ;* ;sub cy. reversed  Rd = Op2-Rn+Cy-1
                    8: TST{cond}{P}    Rn,Op2    ;test            Void = Rn AND Op2
                    9: TEQ{cond}{P}    Rn,Op2    ;test exclusive  Void = Rn XOR Op2
                    A: CMP{cond}{P}    Rn,Op2 ;* ;compare         Void = Rn-Op2
                    B: CMN{cond}{P}    Rn,Op2 ;* ;compare neg.    Void = Rn+Op2
                    C: ORR{cond}{S} Rd,Rn,Op2    ;OR logical        Rd = Rn OR Op2
                    D: MOV{cond}{S} Rd,Op2       ;move              Rd = Op2
                    E: BIC{cond}{S} Rd,Rn,Op2    ;bit clear         Rd = Rn AND NOT Op2
                    F: MVN{cond}{S} Rd,Op2       ;not               Rd = NOT Op2
           20     S - Set Condition Codes (0=No, 1=Yes) (Must be 1 for opcode 8-B)
           19-16  Rn - 1st Operand Register (R0..R15) (including PC=R15)
                       Must be 0000b for MOV/MVN.
           15-12  Rd - Destination Register (R0..R15) (including PC=R15)
                       Must be 0000b (or 1111b) for CMP/CMN/TST/TEQ{P}.
           When above Bit 25 I=0 (Register as 2nd Operand)
             When below Bit 4 R=0 - Shift by Immediate
               11-7   Is - Shift amount   (1-31, 0=Special/See below)
             When below Bit 4 R=1 - Shift by Register
               11-8   Rs - Shift register (R0-R14) - only lower 8bit 0-255 used
               7      Reserved, must be zero  (otherwise multiply or LDREX or undefined)
             6-5    Shift Type (0=LSL, 1=LSR, 2=ASR, 3=ROR)
             4      R - Shift by Register Flag (0=Immediate, 1=Register)
             3-0    Rm - 2nd Operand Register (R0..R15) (including PC=R15)
           When above Bit 25 I=1 (Immediate as 2nd Operand)
             11-8   Is - ROR-Shift applied to nn (0-30, in steps of 2)
             7-0    nn - 2nd Operand Unsigned 8bit Immediate
         */
        uint32_t opcode = instruction & 0x01E00000;
        uint32_t s = instruction & 0x00100000;
        bool shiftsRs = false;
        if ((opcode & 0x01800000) == 0x01000000 && !s) {
//            var r = instruction & 0x00400000;
//            if ((instruction & 0x00B0F000) == 0x0020F000) {
//                // MSR
//                var rm = instruction & 0x0000000F;
//                var immediate = instruction & 0x000000FF;
//                var rotateImm = (instruction & 0x00000F00) >> 7;
//                immediate = (immediate >>> rotateImm) | (immediate << (32 - rotateImm));
//                op = this.armCompiler.constructMSR(rm, r, instruction, immediate, condOp);
//                op.writesPC = false;
//            } else if ((instruction & 0x00BF0000) == 0x000F0000) {
//                // MRS
//                var rd = (instruction & 0x0000F000) >> 12;
//                op = this.armCompiler.constructMRS(rd, r, condOp);
//                op.writesPC = rd == this.PC;
//            }
        } else {
//            // Data processing/FSR transfer
//            var rn = (instruction & 0x000F0000) >> 16;
//            var rd = (instruction & 0x0000F000) >> 12;
//
//            // Parse shifter operand
//            var shiftType = instruction & 0x00000060;
//            var rm = instruction & 0x0000000F;
//            var shiftOp = function() {
//                throw 'BUG: invalid barrel shifter';
//            };
//            if (instruction & 0x02000000) {
//                var immediate = instruction & 0x000000FF;
//                var rotate = (instruction & 0x00000F00) >> 7;
//                if (!rotate) {
//                    shiftOp = this.armCompiler.constructAddressingMode1Immediate(immediate);
//                } else {
//                    shiftOp = this.armCompiler.constructAddressingMode1ImmediateRotate(immediate, rotate);
//                }
//            } else if (instruction & 0x00000010) {
//                var rs = (instruction & 0x00000F00) >> 8;
//                shiftsRs = true;
//                switch (shiftType) {
//                case 0x00000000:
//                    // LSL
//                    shiftOp = this.armCompiler.constructAddressingMode1LSL(rs, rm);
//                    break;
//                case 0x00000020:
//                    // LSR
//                    shiftOp = this.armCompiler.constructAddressingMode1LSR(rs, rm);
//                    break;
//                case 0x00000040:
//                    // ASR
//                    shiftOp = this.armCompiler.constructAddressingMode1ASR(rs, rm);
//                    break;
//                case 0x00000060:
//                    // ROR
//                    shiftOp = this.armCompiler.constructAddressingMode1ROR(rs, rm);
//                    break;
//                }
//            } else {
//                var immediate = (instruction & 0x00000F80) >> 7;
//                shiftOp = this.barrelShiftImmediate(shiftType, immediate, rm);
//            }
//
//            switch (opcode) {
//            case 0x00000000:
//                // AND
//                if (s) {
//                    op = this.armCompiler.constructANDS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructAND(rd, rn, shiftOp, condOp);
//                }
//                break;
//            case 0x00200000:
//                // EOR
//                if (s) {
//                    op = this.armCompiler.constructEORS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructEOR(rd, rn, shiftOp, condOp);
//                }
//                break;
//            case 0x00400000:
//                // SUB
//                if (s) {
//                    op = this.armCompiler.constructSUBS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructSUB(rd, rn, shiftOp, condOp);
//                }
//                break;
//            case 0x00600000:
//                // RSB
//                if (s) {
//                    op = this.armCompiler.constructRSBS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructRSB(rd, rn, shiftOp, condOp);
//                }
//                break;
//            case 0x00800000:
//                // ADD
//                if (s) {
//                    op = this.armCompiler.constructADDS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructADD(rd, rn, shiftOp, condOp);
//                }
//                break;
//            case 0x00A00000:
//                // ADC
//                if (s) {
//                    op = this.armCompiler.constructADCS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructADC(rd, rn, shiftOp, condOp);
//                }
//                break;
//            case 0x00C00000:
//                // SBC
//                if (s) {
//                    op = this.armCompiler.constructSBCS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructSBC(rd, rn, shiftOp, condOp);
//                }
//                break;
//            case 0x00E00000:
//                // RSC
//                if (s) {
//                    op = this.armCompiler.constructRSCS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructRSC(rd, rn, shiftOp, condOp);
//                }
//                break;
//            case 0x01000000:
//                // TST
//                op = this.armCompiler.constructTST(rd, rn, shiftOp, condOp);
//                break;
//            case 0x01200000:
//                // TEQ
//                op = this.armCompiler.constructTEQ(rd, rn, shiftOp, condOp);
//                break;
//            case 0x01400000:
//                // CMP
//                op = this.armCompiler.constructCMP(rd, rn, shiftOp, condOp);
//                break;
//            case 0x01600000:
//                // CMN
//                op = this.armCompiler.constructCMN(rd, rn, shiftOp, condOp);
//                break;
//            case 0x01800000:
//                // ORR
//                if (s) {
//                    op = this.armCompiler.constructORRS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructORR(rd, rn, shiftOp, condOp);
//                }
//                break;
//            case 0x01A00000:
//                // MOV
//                if (s) {
//                    op = this.armCompiler.constructMOVS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructMOV(rd, rn, shiftOp, condOp);
//                }
//                break;
//            case 0x01C00000:
//                // BIC
//                if (s) {
//                    op = this.armCompiler.constructBICS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructBIC(rd, rn, shiftOp, condOp);
//                }
//                break;
//            case 0x01E00000:
//                // MVN
//                if (s) {
//                    op = this.armCompiler.constructMVNS(rd, rn, shiftOp, condOp);
//                } else {
//                    op = this.armCompiler.constructMVN(rd, rn, shiftOp, condOp);
//                }
//                break;
//            }
//            op.writesPC = rd == this.PC;
        }
        
        
    } else {
        
        switch (i) {
            case 0x0A000000:
            {
                /*
                 Branch and Branch with Link (B, BL, BLX_imm)
                 Branch (B) is supposed to jump to a subroutine. Branch with Link is meant to be used to call to a subroutine, return address is then saved in R14.
                   Bit    Expl.
                   31-28  Condition (must be 1111b for BLX)
                   27-25  Must be "101" for this instruction
                   24     Opcode (0-1) (or Halfword Offset for BLX)
                           0: B{cond} label    ;branch            PC=PC+8+nn*4
                           1: BL{cond} label   ;branch/link       PC=PC+8+nn*4, LR=PC+4
                           H: BLX label ;ARM9  ;branch/link/thumb PC=PC+8+nn*4+H*2, LR=PC+4, T=1
                   23-0   nn - Signed Offset, step 4      (-32M..+32M in steps of 4)
                 Branch with Link can be used to 'call' to a sub-routine, which may then 'return' by MOV PC,R14 for example.
                 Execution Time: 2S + 1N
                 Return: No flags affected.
                 */
                int immediate = instruction & 0x00FFFFFF;
                if (immediate & 0x00800000) immediate |= 0xFF000000;

                immediate <<= 2;
                bool link = instruction & 0x01000000;
                if (link) { // BL
                    regs->LR = PC - 4; PC += immediate;
                } else { // B
                    PC += immediate;
                }
                regs->PC = PC;
                
                //op->writesPC = true;
                //op->fixedJump = true;
                
                break;
            }
            default:
                INSTRUCTION_ERROR;
                break;
        }
    }
    
    return true;
}

// 反汇编指令
bool disassemble(uint32_t instruction, uint32_t PC, char* buffer) {
    
    uint32_t i = instruction & 0x0E000000;
    if ((instruction & 0x0FFFFFF0) == 0x012FFF10) {
        INSTRUCTION_ERROR;
    } else if (!(instruction & 0x0C000000) && (i == 0x02000000 || (instruction & 0x00000090) != 0x00000090)) {
        INSTRUCTION_ERROR;
    } else {
        
        switch (i) {
            case 0x0A000000:
            {
                int immediate = instruction & 0x00FFFFFF;
                if (immediate & 0x00800000) immediate |= 0xFF000000;

                immediate <<= 2;
                bool link = instruction & 0x01000000;
                if (link) { // BL
                    //regs->LR = PC - 4; PC += immediate;
                    INSTRUCTION_ERROR;
                } else { // B
                    PC += immediate;
                    sprintf(buffer, "B $%x", PC+8);
                }
                
                break;
            }
            default:
                INSTRUCTION_ERROR;
                break;
        }
    }
    
    return true;
}
