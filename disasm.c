#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <endian.h>
#include <string.h>

uint32_t opcode, rn, rd, rm, rt, shamt, alu_imm, br_addr, dt_addr, op, cond_br_addr, mov_imm;

enum InstructionFormat {
    R,
    I,
    D,
    B,
    CB,
    IW
};

void decode(uint32_t instruction, uint32_t *bprogram);

int main(int argc, char **argv) {
    int fd;
    struct stat buf;
    uint32_t *program, *bprogram;
    size_t i;
    char result[100000];
    int empty = 1;

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
        if (empty) {
            strcpy(result, decode(program[i], bprogram + i));
            empty = 0;
        }
        else {
            strcat(result, decode(program[i], bprogram + i));
        }
	}

	return 0;
}

char* printInstruction()
{
    char str[50];

    if (opcode == 0b10011011000){ //MUL
        sprintf(str, "MUL X%d, X%d, X%d\n", rd, rn, rm);
    }
    else if (opcode == 0b11101011000){ //SUBS
        sprintf(str, "SUBS X%d, X%d, X%d\n", rd, rn, rm);
    }
    else if (opcode == 0b1111000100){ //SUBIS
        sprintf(str, "SUBIS X%d, X%d, #%d\n", rd, rn, alu_imm);
    }
    else if (opcode == 0b1101000100){ //SUBI
        sprintf(str, "SUBI X%d, X%d, #%d\n", rd, rn, alu_imm);
    }
    else if (opcode == 0b1101000100){ //SUB
        sprintf(str, "SUB X%d, X%d, X%d\n", rd, rn, rm);
    }
    else if (opcode == 0b11111000000){ //STUR
        sprintf(str, "STUR X%d, [X%d, #%d]\n", rt, rn, dt_addr);
    }
    else if (opcode == 0b1011001000){ //ORRI
        sprintf(str, "ORRI X%d, X%d, #%d\n", rd, rn, alu_imm);
    }
    else if (opcode == 0b10101010000){ //ORR
        sprintf(str, "ORR X%d, X%d, X%d\n", rd, rn, rm);
    }
    else if (opcode == 0b11010011010){ //LSR
        sprintf(str, "LSR X%d, X%d, #%d\n", rd, rn, shamt);
    }
    else if (opcode == 0b11010011011){ //LSL
        sprintf(str, "LSL X%d, X%d, #%d\n", rd, rn, shamt);
    }
    else if (opcode == 0b11111000010){ //LDUR
        sprintf(str, "LDUR X%d, [X%d, #%d]\n", rt, rn, dt_addr);
    }
    else if (opcode == 0b11111000010){ //LDUR
        sprintf(str, "LDUR X%d, [X%d, #%d]\n", rt, rn, dt_addr);
    }
    else if (opcode == 0b11111111111){ //HALT
        sprintf(str, "HALT\n");
    }
    else if (opcode == 0b11111111110){ //DUMP
        sprintf(str, "DUMP\n");
    }
    else if (opcode == 0b11111111100){ //PRNL
        sprintf(str, "PRNL\n");
    }
    else if (opcode == 0b11111111101){ //PRNT
        sprintf(str, "PRNT X%d\n", rd);
    }
    else if (opcode == 0b10001011000) { // ADD
        sprintf(str, "ADD X%d, X%d, X%d\n", rd, rn, rm);
    }
    else if (opcode == 0b1001000100) { // ADDI
        sprintf(str, "ADDI X%d, X%d, #%d\n", rd, rn, alu_imm);
    }
    else if (opcode == 0b10001010000) { // AND
        sprintf(str, "AND X%d, X%d, X%d\n", rd, rn, rm);
    }
    else if (opcode == 0b1001001000) { // ANDI
        sprintf(str, "ANDI X%d, X%d, #%d\n", rd, rn, alu_imm);
    }
    else if (opcode == 0b000101) { // B NOT GOING TO WORK
        sprintf(str, "B %d\n", br_addr);
    }
    else if (opcode == 0b100101) { // BL NOT GOING TO WORK
        sprintf(str, "BL %d\n", br_addr);
    }
    else if (opcode == 0b11010110000) { // BR NOT GOING TO WORK
        sprintf(str, "BR X%d\n", rd);
    }
    else if (opcode == 0b10110101) { // CBNZ
        sprintf(str, "CBNZ X%d, #%d\n", rt, cond_br_addr);
    }
    else if (opcode == 0b10110100) { // CBZ
        sprintf(str, "CBNZ X%d, #%d\n", rt, cond_br_addr);
    }
    else if (opcode == 0b1001001000) { // EOR
        sprintf(str, "EOR X%d, X%d, X%d\n", rd, rn, rm);
    }
    else if (opcode == 0b1001001000) { // EORI
        sprintf(str, "EOR X%d, X%d, #%d\n", rd, rn, alu_imm);
    }

    return str;
}

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
    else if(format == InstructionFormat.D){
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
    else if(format == InstructionFormat.B){
        uint32_t opcodeMask = (1 << 6) - 1;
        opcode = (instruction >> 26) & opcodeMask;

        uint32_t BRAddressMask = (1 << 26) - 1;
        br_addr = (instruction) & BRAddressMask;
    }
    else if(format == InstructionFormat.CB)
    {
        uint32_t opcodeMask = (1 << 8) - 1;
        opcode = (instruction >> 24) & opcodeMask;

        uint32_t CONDBRAddressMask = (1 << 19) - 1;
        cond_br_addr = (instruction >> 5) & CONDBRAddressMask;

        uint32_t rMask = (1 << 5) - 1;
        rt = (instruction) & rMask;
    }
    else if(format == InstructionFormat.IW){
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

char* decode(uint32_t instruction, uint32_t *bprogram) {

    // last 11 bits for opcode
    uint32_t mask = (1 << 11) - 1;
    // gets the last 11 bits
    uint32_t last_11_bits = (instruction >> 21) & mask;

    // get instruction format
    InstructionFormat format = getInstructionFormat(last_11_bits);

    // extract fields from the instruction
    getInstructionChunks(format);

    // print instruction
    return printInstruction();
}