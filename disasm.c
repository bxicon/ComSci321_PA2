#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <endian.h>

uint32_t opcode, rn, rd, rm, shamt, addr, imm, br_addr, dt_addr;

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

InstructionFormat getInstructionFormat(uint32_t last_11_bits)
{
    if (last_11_bits >= 160 && last_11_bits <= 191)
    {
        return InstructionFormat.B;
    }
    else if (last_11_bits == 448)
    {
        return InstructionFormat.D;
    }
    else if (last_11_bits == 450)
    {
        return InstructionFormat.D;
    }
    else if (last_11_bits >= 672 && last_11_bits <= 679)
    {
        return InstructionFormat.CB;
    }
    else if (last_11_bits >= 712 && last_11_bits <= 713)
    {
        return InstructionFormat.I;
    }
    else if (last_11_bits >= 840 && last_11_bits <= 841)
    {
        return InstructionFormat.I;
    }
    else if (last_11_bits == 960)
    {
        return InstructionFormat.D;
    }
    else if (last_11_bits == 962)
    {
        return InstructionFormat.D;
    }
    else if (last_11_bits == 1104)
    {
        return InstructionFormat.R;
    }
    else if (last_11_bits == 1112)
    {
        return InstructionFormat.R;
    }
    else if (last_11_bits >= 1160 && last_11_bits <= 1161)
    {
        return InstructionFormat.I;
    }
    else if (last_11_bits >= 1168 && last_11_bits <= 1169)
    {
        return InstructionFormat.I;
    }
    else if (last_11_bits >= 1184 && last_11_bits <= 1215)
    {
        return InstructionFormat.B;
    }
    else if (last_11_bits == 1360)
    {
        return InstructionFormat.R;
    }
    else if (last_11_bits == 1368)
    {
        return InstructionFormat.R;
    }
    else if (last_11_bits >= 1416 && last_11_bits <= 1417)
    {
        return InstructionFormat.I;
    }
    else if (last_11_bits >= 1440 && last_11_bits <= 1447)
    {
        return InstructionFormat.CB;
    }
    else if (last_11_bits >= 1448 && last_11_bits <= 1455)
    {
        return InstructionFormat.CB;
    }
    else if (last_11_bits == 1472)
    {
        return InstructionFormat.D;
    }
    else if (last_11_bits == 1476)
    {
        return InstructionFormat.D;
    }
    else if (last_11_bits == 1600)
    {
        return InstructionFormat.D;
    }
	else if (last_11_bits == 1602)
	{
		return InstructionFormat.D;
	}
	else if (last_11_bits == 1616)
	{
		return InstructionFormat.R;
	}
	else if (last_11_bits == 1624)
	{
		return InstructionFormat.R;
	}
    else if (last_11_bits >= 1672 && last_11_bits <= 1673)
    {
        return InstructionFormat.I;
    }
    else if (last_11_bits >= 1684 && last_11_bits <= 1687)
    {
        return InstructionFormat.IM;
    }
	else if (last_11_bits == 1690)
	{
		return InstructionFormat.R;
	}
	else if (last_11_bits == 1691)
	{
		return InstructionFormat.R;
	}
	else if (last_11_bits == 1712)
	{
		return InstructionFormat.R;
	}
 	else if (last_11_bits == 1872)
	{
		return InstructionFormat.R;
	}
 	else if (last_11_bits == 1880)
	{
		return InstructionFormat.R;
	}
    else if (last_11_bits >= 1928 && last_11_bits <= 1929)
    {
        return InstructionFormat.I;
    }
    else if (last_11_bits >= 1936 && last_11_bits <= 1937)
    {
        return InstructionFormat.I;
    }
    else if (last_11_bits >= 1940 && last_11_bits <= 1943)
    {
        return InstructionFormat.IM;
    }
 	else if (last_11_bits == 1984)
	{
		return InstructionFormat.D;
	}
 	else if (last_11_bits == 1986)
	{
		return InstructionFormat.D;
	}

    // something bad happens if you get here
    printf("ERROR");
    return InstructionFormat.R;
}

void GetInstructionChunks(InstructionFormat format)
{
    // set opcode, rn, rd, rm, shamt, addr, imm, br_addr, dt_addr
}

void decode(uint32_t instruction, uint32_t *bprogram) {

    // last 11 bits for opcode
    uint32_t mask = (1 << 11) - 1;
    // gets the last 11 bits
    uint32_t last_11_bits = (instuction >> 21) & mask;

    // get instruction format
    InstructionFormat format = getInstructionFormat(last_11_bits);

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