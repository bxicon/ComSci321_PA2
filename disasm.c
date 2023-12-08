#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <endian.h>

uint32_t opcode, rn, rd, rm, shamt, addr, imm, br_addr, dt_addr;

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

void GetInstructionChunks(InstructionFormat format)
{
    // set opcode, rn, rd, rm, shamt, addr, imm, br_addr, dt_addr
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