#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <endian.h>

void decode(uint32_t instruction, uint32_t *bprogram);

int main(int argc, char **argv) {
    int fd;
    struct stat buf;
    uint32_t *program, *bprogram;
    size_t i;
    int binary = 1; // no clue if this is right

    if (binary) {
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
        // emulate(bprogram, buf.st_size / 4, &m);
    }
	return 0;
}



void decode(uint32_t instruction, uint32_t *bprogram) {
    uint32_t opcode, rn, rd, rm, shamt, addr, imm;

    // 32 bit instruction
    for (i = 31; i >= 0; --i) {
        printf("%"PRIu32, opcode >> i & 1);
    }

    // first 11 bits for opcode
    
    
    // Extract fields from the instruction
    opcode = ...;  // Extract opcode
    rn = ...;      // Extract register Rn
    rd = ...;      // Extract register Rd
    // ... other extractions

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