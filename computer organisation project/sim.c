#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NAME_LENGHT 8 //register names
#define ACTION_LENGTH 8  //function names cahne later OP_LENGTH
#define LABEL_LENGTH 50
#define MAX_LINE_COUNT 100
#define NOTFOUND -1
#define MEM_SIZE 4096
#define REG_SIZE 16
#define REG_LEN 9
#define LINE_LENGTH 6


char reg_arr[REG_SIZE][REG_LEN];

char* int_to_hex(val)
{
	char nep[9];
	strcpy(nep, "00000000");
	sprintf(nep, "%8X", val); 
	int i = 0;
	while (nep[i] == ' ')
	{
		nep[i] = '0';
		i++;
	}
	return nep;
}

char* strstrip(char* line)
{
	size_t s = strlen(line);
	if (s && (line[s - 1] == '\n'))
		line[--s] = '\0';  //check? works for \n at the end, get rid of it
}


int execute(char* op, unsigned int Rd, unsigned int Rs, unsigned int Rt,char *instr, char* immf, int *pc, int*cycles,int* stack_flag) {
	/*input : op - string of lenght 2 "x1x2"
	 Rd,Rs,Rt - all integer for the register address
	 reg_arr - register array[][]
	 instr - instruction, str
	 immf - string of lenght 5 "x4x3x2x1x0" 
	 pc - integer point to PC
	 cycles - integer point to cycles count
	 ftrace - file pointer to trace(s).txt

	 requirements: when storing to mem_arr, word need to  a string "x4x3x2x1x0"
	 also a word load is a string "x4x3x2x1x0" 
	 likewise for the reg_arr, word need to  a string "x7x6x5x4x3x2x1x0"
	 also a word load is a a string "x7x6x5x4x3x2x1x0" 
	 set register R1 to immediate

	 functionalities: execute the current instruction, update PC according the I,R or branching instruction
	  keep clk count in cycles, print to trace(s).txt

	 output: 1 if HALT o\w 0
	*/

	//update cycles 
	(*cycles)++;

	if (strcmp(op, "12") == 0)//halt
	{
		return 1;
	}
	//else not HALT
	if (Rd == 0) {
		printf("can't save to $0, instruction %s", instr);
		exit(5);
	}
	
	char*nep=malloc(9*sizeof(char));
	strcpy(nep, "00000000");
	signed int val1, val2, val3;
	strcpy(nep, reg_arr + Rs);
	val1 = (int)strtoll(nep, NULL, 16);
	strcpy(nep, reg_arr + Rt);
	val2 = (int)strtoll(nep, NULL, 16);

	//Addition
	if (strcmp(op, "00") == 0)
	{
		//reg_arr[Rd] = reg_arr[Rs] + reg_arr[Rt];
		val3 = val1 + val2;
		strcpy(reg_arr[Rd], int_to_hex(val3));
	}
	//subtraction
	else if (strcmp(op, "01") == 0)
	{	//reg_arr[Rd] = reg_arr[Rs] - reg_arr[Rt];
		val3 = val1 - val2;
		strcpy(reg_arr[Rd], int_to_hex(val3));
	}
	//multiply
	else if (strcmp(op, "02") == 0)
	{	//reg_arr[Rd] = reg_arr[Rs] * reg_arr[Rt];
		val3 = val1 * val2;
		strcpy(reg_arr[Rd], int_to_hex(val3));
	}
	//And
	else if (strcmp(op, "03") == 0)
	{	//reg_arr[Rd] = reg_arr[Rs] & reg_arr[Rt];
		val3 = val1 & val2;
		strcpy(reg_arr[Rd], int_to_hex(val3));
	}
	//OR
	else if (strcmp(op, "04") == 0)
	{  	//reg_arr[Rd] = reg_arr[Rs] | reg_arr[Rt];
		val3 = val1 | val2;
		strcpy(reg_arr[Rd], int_to_hex(val3));
	}
	//XOR
	else if (strcmp(op, "05") == 0)
	{ 	//reg_arr[Rd] = reg_arr[Rs] ^ reg_arr[Rt];
		val3 = val1 ^ val2;
		strcpy(reg_arr[Rd], int_to_hex(val3));
	}
	//SLL
	else if (strcmp(op, "06") == 0)
	{	//reg_arr[Rd] = reg_arr[Rs] << reg_arr[Rt];
		val2 = (int)strtol(reg_arr[Rt], NULL, 16); // in case it contained by mistake a negative number
		val3 = val1 << val2;
		strcpy(reg_arr[Rd], int_to_hex(val3));

	}
	//Arithmetic shift with sign extension
	else if (strcmp(op, "07") == 0)
	{
		//reg_arr[Rd] = reg_arr[Rs] >> reg_arr[Rt];
		val2 = (int)strtol(reg_arr[Rt], NULL, 16);
		val3 = val1 >> val2;
		strcpy(reg_arr[Rd], int_to_hex(val3));

	}
	//SRL
	else if (strcmp(op, "08") == 0)
	{
		val1 = (int)strtol(reg_arr[Rs], NULL, 16);
		val2 = (int)strtol(reg_arr[Rt], NULL, 16);
		val3 = (unsigned int)val1 >> val2;
		strcpy(reg_arr[Rd], int_to_hex(val3));
	}
	else if (strcmp(op, "09")==0) {//beq
		if (val1 == val2) {
			strcpy(nep, reg_arr + Rd);
			int temp = (int)strtol(nep, NULL, 16);
			if (temp >= 4096) {
				printf("Can't assign PC a value greater than 4095");
				exit(20);
			}
			*pc = temp;
		}
	}
	else if (strcmp(op, "0A") == 0) {//bne
		if (val1 != val2) {
			strcpy(nep, reg_arr + Rd);
			int temp = (int)strtol(nep, NULL, 16);
			if (temp >= 4096) {
				printf("Can't assign PC a value greater than 4095");
				exit(21);
			}
			*pc = temp;
		}
	}
	else if (strcmp(op, "0B") == 0) {//blt
		if (val1 < val2) {
			strcpy(nep, reg_arr + Rd);
			int temp = (int)strtol(nep, NULL, 16);
			if (temp >= 4096) {
				printf("Can't assign PC a value greater than 4095");
				exit(22);
			}
			*pc = temp;
		}

	}
	else if (strcmp(op, "0C") == 0) {//bgt
		if (val1 > val2) {
			strcpy(nep, reg_arr + Rd);
			int temp = (int)strtol(nep, NULL, 16);
			if (temp >= 4096) {
				printf("Can't assign PC a value greater than 4095");
				exit(23);
			}
			*pc = temp;
		}
	}
	else if (strcmp(op, "0D") == 0) {//ble
		if (val1 <= val2) {
			strcpy(nep, reg_arr + Rd);
			int temp = (int)strtol(nep, NULL, 16);
			if (temp >= 4096) {
				printf("Can't assign PC a value greater than 4095");
				exit(24);
			}
			*pc = temp;
		}
	}
	else if (strcmp(op, "0E") == 0) {//bge
		if (val1 >= val2) {
			strcpy(nep, reg_arr + Rd);
			int temp = (int)strtol(nep, NULL, 16);
			if (temp >= 4096) {
				printf("Can't assign PC a value greater than 4095");
				exit(25);
			}
			*pc = temp;
		}
	}
	else if (strcmp(op, "0F") == 0) {//jal	
		strcpy(nep, reg_arr + Rs);
		int temp = (int)strtol(nep, NULL, 16);
		strcpy(reg_arr + Rd, int_to_hex(*pc)); //R[Rd]=pc+1 or +2 which is already computed above
		if (temp >= 4096) {
			printf("Can't assign PC a value greater than 4095");
			exit(26);
		}
		*pc = temp;//pc=R[Rs]

	}
	//turn .word is done in assembler
	return 0;
}

	int main(int argc,char* argv[]) {
		FILE* fmem, *fmemout, *fregout, *ftrace, *fcycle;
		fmem = fopen(argv[1], "r");
		fmemout = fopen(argv[2], "w");
		fregout = fopen(argv[3], "w");
		//update proper file addresses
		ftrace = fopen(argv[4], "w");
		fcycle = fopen(argv[5], "w");

		//update to arg for cmd use
		unsigned int i;
		int cycles = 0;
		char mem_arr[MEM_SIZE][LINE_LENGTH];
		//init array for the mem_in
		for (i = 0; i < MEM_SIZE; i++) {
			strcpy(mem_arr[i], "00000");
		}

		char line[LINE_LENGTH+1];
		i = 0;
		int j = 0;
		while (NULL != fgets(line, LINE_LENGTH+1, fmem)) {
			if (strlen(line) != 6) {//including \n
				printf("invalid line length, instead of 5, %d is provided", strlen(line));
				exit(1);
			}
			 size_t s = strlen(line);
			 if (s && (line[s - 1] == '\n'))
			 line[--s] = '\0';

			for (j = 0; j < 5; j++) {
				if (line[j] >= 97 && line[j] <= 122)
					line[j] = toupper(line[j]);
			}
			strcpy(mem_arr + i, line);
			i++;
		}
		//create register arr
		//char reg_arr[REG_SIZE][REG_LEN];
		//init array for the mem_in
		for (i = 0; i < REG_SIZE; i++) {
			strcpy(reg_arr[i], "000000000");
		}
		//magic!
		unsigned int pc = 0;
		cycles = 0;
		int ret = 0;
		int stack_flag = 0;
		while (ret != 1) {
			//Instr fetch
			char instr[LINE_LENGTH];
			strcpy(instr, mem_arr[pc]);
			char immf[LINE_LENGTH];
			strcpy(immf, "00000");
			if ((instr[4] =='1')|| (instr[3] == '1')|| (instr[2] == '1')) { //using the $imm register so load next line for imm value
				strcpy(immf, mem_arr[pc + 1]);
			}

			//instr decode;
			char op[3];
			strncpy(op, instr, 2);
			op[2] = '\0';//check??

			char nep[REG_LEN];
			char nep2[REG_LEN];
			strncpy(nep, instr + 2, 2);
			nep[1] = '\0';
			int Rd = (int)strtol(nep, NULL, 16);

			strncpy(nep, instr + 3, 2);
			nep[1] = '\0';
			int Rs = (int)strtol(nep, NULL, 16);

			strncpy(nep, instr + 4, 2);
			nep[1] = '\0';
			int Rt = (int)strtol(nep, NULL, 16);
			//update PC and cycles(partly)
			//load Imm to register $1
			if ((instr[4] == '1') || (instr[3] == '1') || (instr[2] == '1'))
			{	
				strcpy(nep, "00000000");
				strcpy(nep + 3, immf);
				nep[0] = nep[3];
				nep[1] = nep[3];
				nep[2] = nep[3]; //signed extension before using strtoll
				strcpy(reg_arr + 1, nep); 
				cycles++;
				pc = pc + 2;
			}
			else
			{// o/w $imm is always 00000000  (ofc only in str)
				pc++;
			}
			
			//print traces
			fprintf(ftrace, "%03x ", pc);
			fprintf(ftrace, "%05s ", instr);
			i = 0;
			for (i = 0; i < 16; i++)
			{
				if (i == 15)
				{
					fprintf(ftrace, "%08s\n", reg_arr[i]);
				}
				else
				{
					fprintf(ftrace, "%08s ", reg_arr[i]);
				}
			}
			//stack condition
			if ((strcmp(op, "00")==0) && (Rs == 14) && (Rd == 14)) { // stack
				//create a stack_arr
				strcpy(nep, "00000000");
				strcpy(nep + 3, immf);
				nep[0] = nep[3];
				nep[1] = nep[3];
				nep[2] = nep[3];
				signed int immv = (int)strtoll(nep, NULL, 16);
				int sp_val = (int)strtol(reg_arr[14], NULL, 16); // get the address to the bottom of the stack 
				if (immv > 0) {//free stack
					int ik = 0;
					for (ik = 0;ik < immv;ik++) {
						strcpy(mem_arr[sp_val + ik], "00000");
					}
				}
				else {
					stack_flag = -immv;
				}

			}
			int val1 = 0;
			//if sw or lw
			if (strcmp(op, "10") == 0) {//lw
				cycles = cycles + 1;
				strcpy(nep, reg_arr + Rs);
				val1 = (int)strtol(nep, NULL, 16);
				strcpy(nep, reg_arr + Rt);
				val1 += (int)strtol(nep, NULL, 16);
				strcpy(nep, mem_arr[val1]);
				//+++
				strcpy(nep2, "00000000");
				strcpy(nep2 + 3, nep);
				nep2[0] = nep2[3];
				nep2[1] = nep2[3];
				nep2[2] = nep2[3];
				strcpy(reg_arr + Rd,nep2);
			}
			else if (strcmp(op, "11") == 0) {//sw
				cycles = cycles + 1;
				if (Rd == 0)
					printf("Error: writing to $0 is not allowed!!!"), exit(6);
				else {
					if ((Rs == 14) && (stack_flag == 0)) { // this is a stack operation
						printf("Error! stack overflow, no space on stack to store"), exit(3);
					}
					else {
						strcpy(nep, reg_arr + Rs);
						val1 = (int)strtol(nep, NULL, 16);
						strcpy(nep, reg_arr + Rt);
						val1 += (int)strtol(nep, NULL, 16);
						strcpy(nep, reg_arr + Rd);
						strncpy(mem_arr[val1], nep + 3, LINE_LENGTH - 1);
						if (Rs == 14)
							stack_flag--;
					}
				}
			}
			//else add parameters to execute
			else {
				ret = execute(op, Rd, Rs, Rt, instr, immf, &pc, &cycles, &stack_flag);
			}
	
		}

		//ouputs
		//use argv[3?]
		//1-memout
		for (i = 0; i < MEM_SIZE; i++) {
			fprintf(fmemout, "%s\n", mem_arr[i]);
		}
		//2-reg_out
		for (i = 2; i < REG_SIZE; i++) {
			fprintf(fregout, "%s\n", reg_arr[i]);
		}
		//3-cycles
		fprintf(fcycle, "%d", cycles);
		//4-trace is updated in execute

		fclose(fmem);
		fclose(fmemout);
		fclose(ftrace);
		fclose(fcycle);
		fclose(fregout);
		exit(0);
	}