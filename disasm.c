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
    int binary = 1; //No clue if this is right

    if (binary) {
        fd = open(argv[1], O_RDONLY);
        fstat(fd, &buf);
        program = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE,
            MAP_PRIVATE, fd, 0);
        bprogram = calloc(buf.st_size / 4, sizeof (*bprogram));
        for (i = 0; i < (buf.st_size / 4); i++) {
             program[i] = be32toh(program[i]);
             decode(program[i], bprogram + i);
         }
        //emulate(bprogram, buf.st_size / 4, &m);
        return 0;
 }
}

void decode(uint32_t instruction, uint32_t *bprogram) {
    uint32_t opcode, rn, rd, rm, shamt, addr, imm;
    
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