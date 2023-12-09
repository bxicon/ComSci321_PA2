#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <endian.h>

uint32_t opcode, rn, rd, rm, rt, shamt, alu_imm, br_addr, dt_addr, op, cond_br_addr, mov_imm;

instruction_t instruction[] = {
  { "ADD",     ADD_inst,    0b10001011000 },
  { "ADDI",    ADDI_inst,   0b1001000100  },
  { "ADDIS",   ADDIS_inst,  0b1011000100  },
  { "ADDS",    ADDS_inst,   0b10101011000 },
  { "AND",     AND_inst,    0b10001010000 },
  { "ANDI",    ANDI_inst,   0b1001001000  },
  { "ANDIS",   ANDIS_inst,  0b1111001000  },
  { "ANDS",    ANDS_inst,   0b1110101000  },
  { "B",       B_inst,      0b000101      },
  { "BL",      BL_inst,     0b100101      },
  { "BR",      BR_inst,     0b11010110000 },
  { "CBNZ",    CBNZ_inst,   0b10110101    },
  { "CBZ",     CBZ_inst,    0b10110100    },
  { "DUMP",    DUMP_inst,   0b11111111110 },
  { "EOR",     EOR_inst,    0b11001010000 },
  { "EORI",    EORI_inst,   0b1101001000  },
  { "FADDD",   FADDD_inst,  0b00011110011 },
  { "FADDS",   FADDS_inst,  0b00011110001 },
  { "FCMPD",   FCMPD_inst,  0b00011110011 },
  { "FCMPS",   FCMPS_inst,  0b00011110001 },
  { "FDIVD",   FDIVD_inst,  0b00011110011 },
  { "FDIVS",   FDIVS_inst,  0b00011110001 },
  { "FMULD",   FMULD_inst,  0b00011110011 },
  { "FMULS",   FMULS_inst,  0b00011110001 },
  { "FSUBD",   FSUBD_inst,  0b00011110011 },
  { "FSUBS",   FSUBS_inst,  0b00011110001 },
  { "HALT",    HALT_inst,   0b11111111111 },
  { "LDUR",    LDUR_inst,   0b11111000010 },
  { "LDURB",   LDURB_inst,  0b00111000010 },
  { "LDURD",   LDURD_inst,  0b11111100010 },
  { "LDURH",   LDURH_inst,  0b01111000010 },
  { "LDURS",   LDURS_inst,  0b10111100010 },
  { "LDURSW",  LDURSW_inst, 0b10111000100 },
  { "LSL",     LSL_inst,    0b11010011011 },
  { "LSR",     LSR_inst,    0b11010011010 },
  { "MUL",     MUL_inst,    0b10011011000 },
  { "ORR",     ORR_inst,    0b10101010000 },
  { "ORRI",    ORRI_inst,   0b1011001000  },
  { "PRNL",    PRNL_inst,   0b11111111100 },
  { "PRNT",    PRNT_inst,   0b11111111101 },
  { "SDIV",    SDIV_inst,   0b10011010110 },
  { "SMULH",   SMULH_inst,  0b10011011010 },
  { "STUR",    STUR_inst,   0b11111000000 },
  { "STURB",   STURB_inst,  0b00111000000 },
  { "STURD",   STURD_inst,  0b11111100000 },
  { "STURH",   STURH_inst,  0b01111000000 },
  { "STURS",   STURS_inst,  0b10111100000 },
  { "STURSW",  STURW_inst,  0b10111000000 },
  { "SUB",     SUB_inst,    0b11001011000 },
  { "SUBI",    SUBI_inst,   0b1101000100  },
  { "SUBIS",   SUBIS_inst,  0b1111000100  },
  { "SUBS",    SUBS_inst,   0b11101011000 },
  { "UDIV",    UDIV_inst,   0b10011010110 },
  { "UMULH",   UMULH_inst,  0b10011011110 }
};

void decode(uint32_t instruction, uint32_t *bprogram);

int main(int argc, char **argv) {
    int fd;
    struct stat buf;
    uint32_t *program, *bprogram;
    size_t i;

	// get file handle index
	fd = open(argv[1], O_RDONLY);

	// fill buf with file result
	fstat(fd, &buf);

	// program points to a block of memory that is mapped to the specific file
	program = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

	// idk
	bprogram = calloc(buf.st_size / 4, sizeof (*bprogram));

	for (i = 0; i < (buf.st_size / 4); i++) {
		// converts instruction to linux os binary format
		program[i] = be32toh(program[i]);

		// takes 32bit instruction and make out brian
		decode(program[i], bprogram + i);
	}

	// prints out the bprogram
	// emulate(bprogram, buf.st_size / 4, &m);

	return 0;
}

enum InstructionFormat {
    R,
    I,
    D,
    B,
    CB,
    IW
};

InstructionFormat getInstructionFormation(uint32_t first_11_bits)
{
    switch (first_11_bits)
    {
    case 69:
        return R;
    }

    return InstructionFormat.R;
}

void GetInstructionChunks(InstructionFormat format, uint32_t instruction)
{
    // set opcode, rn, rd, rm, shamt, addr, imm, br_addr, dt_addr

    if(format == InstructionFormat.R){
        uint32_t opcodeMask = (1 << 11) - 1;
        opcode = (instruction >> 21) & opcodeMask;

        uint32_t rMask = (1 << 5) - 1;
        rm = (instruction >> 16) & rMask;

        uint32_t shamtMask = (1 << 6) - 1;
        shamt = (instruction >> 10) & shamtMask;

        rn = (instruction >> 5) & rMask;

        rd = (instruction) & rMask;
    }
    else if(format == InstructionFormat.I){
        uint32_t opcodeMask = (1 << 10) - 1;
        opcode = (instruction >> 22) & opcodeMask;

        uint32_t AluImmMask = (1 << 12) - 1;
        alu_imm = (instruction >> 10) & AluImmMask;

        uint32_t rMask = (1 << 5) - 1;
        rn = (instruction >> 5) & rMask;

        rd = (instruction) & rMask;
    }
    else if(format = InstructionFormat.D){
        uint32_t opcodeMask = (1 << 11) - 1;
        opcode = (instruction >> 21) & opcodeMask;  

        uint32_t DTAddressMask = (1 << 9) - 1;
        dt_addr = (instruction >> 12) & DTAddressMask; 

        uint32_t opMask = (1 << 2) - 1;
        op = (instruction >> 10) & opMask; 

        uint32_t rMask = (1 << 5) - 1;
        rn = (instruction >> 5) & rMask;

        rt = (instruction) & rMask;
    }
    else if(format = InstructionFormat.B){
        uint32_t opcodeMask = (1 << 6) - 1;
        opcode = (instruction >> 26) & opcodeMask;

        uint32_t BRAddressMask = (1 << 26) - 1;
        br_addr = (instruction) & BRAddressMask;
    }
    else if(format = InstructionFormat.CB)
    {
        uint32_t opcodeMask = (1 << 8) - 1;
        opcode = (instruction >> 24) & opcodeMask;

        uint32_t CONDBRAddressMask = (1 << 19) - 1;
        cond_br_addr = (instruction >> 5) & CONDBRAddressMask;

        uint32_t rMask = (1 << 5) - 1;
        rt = (instruction) & rMask;
    }
    else if(format = InstructionFormat.IW){
        uint32_t opcodeMask = (1 << 11) - 1;
        opcode = (instruction >> 21) & opcodeMask;

        uint32_t MOvImmMask = (1 << 16) - 1;
        mov_imm = (instruction >> 5) & MOvImmMask;

        uint32_t rMask = (1 << 5) - 1;
        rd = (instruction) & rMask;
    }
    else{
        printf(":(");
    }
}

void decode(uint32_t instruction, uint32_t *bprogram) {

    // first 11 bits for opcode
    uint32_t mask = (1 << 11) - 1;
    // gets the first 11 bits
    uint32_t first_11_bits = instuction & mask;

    // get instruction format
    InstructionFormat format = getInstructionFormat(first_11_bits);

    // extract fields from the instruction
    getInstructionChunks(format);

    // Use opcode to determine the instruction
    switch (opcode) {
    case OPCODE_ADD:
        // Handle ADD instruction
        break;
    // ... other cases
    default:
        printf("Unknown instruction: 0x%x\n", instruction);
    }
}