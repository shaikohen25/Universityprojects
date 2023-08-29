#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LINE_NUMBER 4096
#define NAME_LENGTH 8 //register names
#define LINE_LENGTH 300
#define ACTION_LENGTH 8  //function names cahne later OP_LENGTH
#define LABEL_LENGTH 50
#define MAX_LINE_COUNT 100
#define NOTFOUND -1

typedef struct labels {    //data of type label for the labels
	char name[LABEL_LENGTH];
	int addressoflabel;
	struct labels* next;
}labels;
typedef struct codeline
{
	char opcode[ACTION_LENGTH],//if there is a label, we store the label here and fill all the registers with the "1111" string
		reg_rd[ACTION_LENGTH],
		reg_rs[ACTION_LENGTH],
		reg_rt[ACTION_LENGTH],
		imm[LABEL_LENGTH];
	int islabel, //0 if not label, 1 if label
		addressoflabel;//0 if not label, 1 if label initially then address of label later
	struct codeline* next;
}CODELINE;
CODELINE* add_codeline(CODELINE* head, CODELINE* new_node) //add to the last
{
	CODELINE* iter, * prev = NULL;
	if (head == NULL)
	{
		new_node->next = head;
		return new_node;//New head since first
	}
	for (iter = head; iter != NULL; iter = iter->next) //go to the last
		prev = iter;
	prev->next = new_node;
	new_node->next = iter;
	return head;
}
labels* add_label(labels* head, labels* new_node) //add label to the last position
{
	labels* iter, * prev = NULL;
	if (head == NULL)
	{
		new_node->next = head;
		return new_node;//New head since first
	}
	for (iter = head; iter != NULL; iter = iter->next) //go to the last
		prev = iter;
	prev->next = new_node;
	new_node->next = iter;
	return head;
}

void address_locator(CODELINE* headc, labels* headl) //function that receives the head of the linked list of instructions and of the labels list
{
	for (labels* iter1 = headl; iter1 != NULL; iter1 = iter1->next)
	{
		int count = 0;
		for (CODELINE* iter = headc; iter != NULL; iter = iter->next)
		{
			if (strcmp(iter->opcode, iter1->name) == 0)
			{
				iter->addressoflabel = count;
				iter1->addressoflabel = count;
			}
			if (iter->islabel == 1)
			{
				continue;
			}
			if ((strcmp(iter->opcode, "sw") == 0) || (strcmp(iter->opcode, "lw") == 0) || (strcmp(iter->imm, "0") != 0))
				count = count + 2;
			else
				count = count + 1;
		}
	}
}
void address_allocator(CODELINE* headc, labels* headl)
{
	for (labels* iter1 = headl; iter1 != NULL; iter1 = iter1->next)
	{
		for (CODELINE* iter = headc; iter != NULL; iter = iter->next)
		{
			if (strcmp(iter->imm, iter1->name) == 0)
			{
				char address[NAME_LENGTH];
				sprintf(address, "%d", iter1->addressoflabel);
				strcpy(iter->imm, address);
			}
		}
	}
}
void free_list_code(CODELINE* head)
{
	CODELINE* iter;
	while (head != NULL)
	{
		iter = head;
		head = head->next;
		free(iter);
	}
}
void free_list_lab(labels* head)
{
	labels* iter;
	while (head != NULL)
	{
		iter = head;
		head = head->next;
		free(iter);
	}
}
int coloncheck(const char* line) // this function checks if we have a colon i.e. ':' in our input line and if we do it creates a label
{
	int i = 0;
	for (i = 0; line[i] != '\0'; i++)
	{
		if (line[i] == ':')
			return 0;
	}
	return 1;
}
void initialize_label(CODELINE* line) // this function makes the code less bulky by initializing the labels for us
{
	strcpy(line->reg_rd, "1111");
	strcpy(line->reg_rs, "1111");
	strcpy(line->reg_rt, "1111");
	strcpy(line->imm, "1111");
	line->islabel = 1;
	line->addressoflabel = 1;
}
void unpackline(const char* input, CODELINE* line, labels* label) //this function creates the linked list of instructions and the linked list of labels
{
	int opcount = 0, rdcount = 0, rscount = 0, rtcount = 0, immcount = 0, backtcount = 0, wordcount = 0;
	char opcode[ACTION_LENGTH],
		rd[ACTION_LENGTH],
		rs[ACTION_LENGTH],
		rt[ACTION_LENGTH],
		imm[LABEL_LENGTH];
	if (coloncheck(input) == 0) //if we have a colon, we make a label
	{
		int i = 0, countlab = 0;
		for (i = 0; input[i] != ':'; i++)
		{
			opcode[countlab] = input[i];
			countlab++;
		}
		opcode[countlab] = '\0';
		strcpy(label->name, opcode);
		label->addressoflabel = 1;
		strcpy(line->opcode, opcode);
		initialize_label(line);
	}
	else if (input[1] == '.') // if we have a dot on the second element in the input then we have a .word instruction
	{
		int i = 0;
		for (i = 0; input[i] != '\n'; i++)
		{
			if (input[i] == ' ')
			{
				wordcount++;
				continue;
			}
			else if (input[i] == '\t')
			{
				continue;
			}
			else
			{
				if (wordcount == 0)
				{
					opcode[opcount] = input[i];
					opcount++;
				}
				else if (wordcount == 1)
				{
					rd[rdcount] = input[i];
					rdcount++;
				}
				else if (wordcount == 2)
				{
					rs[rscount] = input[i];
					rscount++;
				}
			}


		}
		opcode[opcount] = '\0';
		rd[rdcount] = '\0';
		rs[rscount] = '\0';
		strcpy(line->opcode, opcode);
		strcpy(line->reg_rd, rd);
		strcpy(line->reg_rs, rs);
		strcpy(line->reg_rt, "1111");
		strcpy(line->imm, "1111");
		line->islabel = 0;
		line->addressoflabel = 0;
	}
	else //then lastly if the above 2 don't hold then evidently we have a regular instrcution and we proceed as planned
	{
		int i = 0;
		for (i = 0; input[i] != '\n'; i++)
		{
			if (backtcount == 2)
			{
				break;
			}
			if (input[i] == ',')
			{
				continue;
			}
			if (input[i] == '\t')
			{
				backtcount++;
				continue;
			}
			if (input[i] == ' ')
			{
				wordcount++;
				continue;
			}
			else
			{
				if (wordcount == 0)
				{
					opcode[opcount] = input[i];
					opcount++;
				}
				else if (wordcount == 1)
				{
					rd[rdcount] = input[i];
					rdcount++;
				}
				else if (wordcount == 2)
				{
					rs[rscount] = input[i];
					rscount++;
				}
				else if (wordcount == 3)
				{
					rt[rtcount] = input[i];
					rtcount++;
				}
				else if (wordcount == 4)
				{
					imm[immcount] = input[i];
					immcount++;
				}
			}
		}
		opcode[opcount] = '\0';
		rd[rdcount] = '\0';
		rs[rscount] = '\0';
		rt[rtcount] = '\0';
		imm[immcount] = '\0';
		strcpy(line->opcode, opcode);
		strcpy(line->reg_rd, rd);
		strcpy(line->reg_rs, rs);
		strcpy(line->reg_rt, rt);
		strcpy(line->imm, imm);
		line->islabel = 0;
		line->addressoflabel = 0;
	}
}
//19 statements to decode the opcode
void decode_opcode(CODELINE* head, char* opcode)
{
	if (strcmp(head->opcode, "add") == 0)
	{
		strcpy(opcode, "00");
	}
	else if (strcmp(head->opcode, "sub") == 0)
	{
		strcpy(opcode, "01");
	}
	else if (strcmp(head->opcode, "mul") == 0)
	{
		strcpy(opcode, "02");
	}
	else if (strcmp(head->opcode, "and") == 0)
	{
		strcpy(opcode, "03");
	}
	else if (strcmp(head->opcode, "or") == 0)
	{
		strcpy(opcode, "04");
	}
	else if (strcmp(head->opcode, "xor") == 0)
	{
		strcpy(opcode, "05");
	}
	else if (strcmp(head->opcode, "sll") == 0)
	{
		strcpy(opcode, "06");
	}
	else if (strcmp(head->opcode, "sra") == 0)
	{
		strcpy(opcode, "07");
	}
	else if (strcmp(head->opcode, "srl") == 0)
	{
		strcpy(opcode, "08");
	}
	else if (strcmp(head->opcode, "beq") == 0)
	{
		strcpy(opcode, "09");
	}
	else if (strcmp(head->opcode, "bne") == 0)
	{
		strcpy(opcode, "0A");
	}
	else if (strcmp(head->opcode, "blt") == 0)
	{
		strcpy(opcode, "0B");
	}
	else if (strcmp(head->opcode, "bgt") == 0)
	{
		strcpy(opcode, "0C");
	}
	else if (strcmp(head->opcode, "ble") == 0)
	{
		strcpy(opcode, "0D");
	}
	else if (strcmp(head->opcode, "bge") == 0)
	{
		strcpy(opcode, "0E");
	}
	else if (strcmp(head->opcode, "jal") == 0)
	{
		strcpy(opcode, "0F");
	}
	else if (strcmp(head->opcode, "lw") == 0)
	{
		strcpy(opcode, "10");
	}
	else if (strcmp(head->opcode, "sw") == 0)
	{
		strcpy(opcode, "11");
	}
	else if (strcmp(head->opcode, "halt") == 0)
	{
		strcpy(opcode, "12");
	}
}
// 16 statements in a fucntion to decode the registers
void decode_assembly_register(char* rg_in, char* rg_out)
{
	if (strcmp(rg_in, "$zero") == 0)
	{
		strcpy(rg_out, "0");
	}
	else if (strcmp(rg_in, "$imm") == 0)
	{
		strcpy(rg_out, "1");
	}
	else if (strcmp(rg_in, "$v0") == 0)
	{
		strcpy(rg_out, "2");
	}
	else if (strcmp(rg_in, "$a0") == 0)
	{
		strcpy(rg_out, "3");
	}
	else if (strcmp(rg_in, "$a1") == 0)
	{
		strcpy(rg_out, "4");
	}
	else if (strcmp(rg_in, "$a2") == 0)
	{
		strcpy(rg_out, "5");
	}
	else if (strcmp(rg_in, "$a3") == 0)
	{
		strcpy(rg_out, "6");
	}
	else if (strcmp(rg_in, "$t0") == 0)
	{
		strcpy(rg_out, "7");
	}
	else if (strcmp(rg_in, "$t1") == 0)
	{
		strcpy(rg_out, "8");
	}
	else if (strcmp(rg_in, "$t2") == 0)
	{
		strcpy(rg_out, "9");;
	}
	else if (strcmp(rg_in, "$s0") == 0)
	{
		strcpy(rg_out, "A");
	}
	else if (strcmp(rg_in, "$s1") == 0)
	{
		strcpy(rg_out, "B");
	}
	else if (strcmp(rg_in, "$s2") == 0)
	{
		strcpy(rg_out, "C");
	}
	else if (strcmp(rg_in, "$gp") == 0)
	{
		strcpy(rg_out, "D");
	}
	else if (strcmp(rg_in, "$sp") == 0)
	{
		strcpy(rg_out, "E");
	}
	else if (strcmp(rg_in, "$ra") == 0)
	{
		strcpy(rg_out, "F");
	}
}
//below function converts a string to a number in decimal so that we can print it in hex
int process_imm(CODELINE* head)
{
	int num = (int)strtol(head->imm, NULL, 10);
	return num;
}
void limit_string(const char* input, char* output)
{
	int i = 0;
	for (i = 0; input[i] != '\0'; i++)
	{
		output[i] = input[i + 3];
	}
	output[i] = '\0';
}
void stringconcat(char* opcode, char* rd, char* rs, char* rt, char* printed)
{
	char* right[LINE_LENGTH];
	strcpy(printed, opcode);
	strcpy(right, rs);
	strcat(printed, rd);
	strcat(right, rt);
	strcat(printed, right);
	strcat(printed, "\n");
}
void shift_string(char sentence[], int i, int num) /*Helper function that shifts the string accordingly and thus gives the notion of "deleting the string" */
{
	int j;
	j = i;
	while (sentence[j] != '\0')
	{
		sentence[j] = sentence[j + num];
		j++;
	}
	sentence[j] = '\0';
}
void processword(CODELINE* iter)
{
	shift_string(iter->reg_rd, 0, 2);
	shift_string(iter->reg_rs, 0, 2);
}


void write_to_array(CODELINE* head, char** lineArray)
{
	int counter = 0;
	CODELINE* iter = head;
	while (counter < 60)
	{
		if (iter != NULL)
		{
			if (iter->islabel == 1)
			{
				iter = iter->next;
				continue;
			}
			else if (((strcmp(iter->opcode, "sw") == 0) || (strcmp(iter->opcode, "lw") == 0) || (strcmp(iter->imm, "0") != 0)) && (counter < 4095)&&(strcmp(iter->opcode,".word")!=0)) //more like you should check if it's an I-type instruction
			{
				char rd[2], rs[2], rt[2], opcode[3],printed[2*LINE_LENGTH];
				int num;
				decode_opcode(iter, opcode);
				decode_assembly_register(iter->reg_rd, rd);
				decode_assembly_register(iter->reg_rs, rs);
				decode_assembly_register(iter->reg_rt, rt);
				stringconcat(opcode, rd, rs, rt, printed);
				strcpy(lineArray[counter], printed);
				if (iter->imm[0] == '-')
				{
					char word1[50], word2[50];
					num = process_imm(iter);
					sprintf(word1, "%05X", num);
					limit_string(word1, word2);
					strcat(word2, "\n");
					strcpy(lineArray[counter + 1], word2);
				}
				else
				{
					char word1[50];
					num = process_imm(iter);
					sprintf(word1, "%05X", num);
					strcat(word1, "\n");
					strcpy(lineArray[counter + 1], word1);
				}
				counter += 2;
			}
			else if (strcmp(iter->opcode, ".word") == 0)
			{
				if (iter->reg_rd[1] == 'x')
				{
					char word1[50];
					processword(iter);
					int num1, num2;
					num1 = (int)strtol(iter->reg_rd, NULL, 16);
					num2 = (int)strtol(iter->reg_rs, NULL, 16);
					sprintf(word1, "%05X", num2);
					strcat(word1, "\n");
					strcpy(lineArray[num1], word1);
				}
				else
				{
					char word1[50];
					int num1, num2;
					num1 = (int)strtol(iter->reg_rd, NULL, 10);
					num2 = (int)strtol(iter->reg_rs, NULL, 10);
					sprintf(word1, "%05X", num2);
					strcat(word1, "\n");
					strcpy(lineArray[num1], word1);
				}
			}

			else
			{
				char rd[2], rs[2], rt[2], opcode[3], printed[2*LINE_LENGTH];
				decode_opcode(iter, opcode);
				decode_assembly_register(iter->reg_rd, rd);
				decode_assembly_register(iter->reg_rs, rs);
				decode_assembly_register(iter->reg_rt, rt);
				stringconcat(opcode, rd, rs, rt, printed);
				strcpy(lineArray[counter], printed);
				counter++;
			}
			iter = iter->next;
		}
		else if (strcmp(lineArray[counter], "00000\n") == 0)
		{
			counter++;
			continue;
		}
	}
}
void initialize_array(char** linesArray)
{
	int i = 0;
	for (i = 0; i < LINE_NUMBER; i++)
	{
		strcpy(linesArray[i], "00000\n");
	}
}
void write_to_file(FILE* memin, char** linesArray)
{
	int i = 0;
	for (i = 0; i < LINE_NUMBER; i++)
	{
		fprintf(memin, "%s", linesArray[i]);
	}
}
int main(int argc, char* argv[]) {
	FILE* asmb;
	asmb = fopen(argv[1], "r");
	if (asmb == NULL) {
		printf("Error opening the file");
		exit(1);
	}
	CODELINE* head_codeline = NULL;
	labels* head_lab = NULL;
	char new_line[LINE_LENGTH];
	while (fgets(new_line, LINE_LENGTH, asmb) != NULL)
	{
		labels* new_label = (labels*)malloc(sizeof(labels));
		if (new_label == NULL) {
			printf("Error: memory allocation failed\n");
			free_list_lab(head_lab);
			exit(1);
		}
		CODELINE* line = (CODELINE*)malloc(sizeof(CODELINE));
		if (line == NULL) {
			printf("Error: memory allocation failed\n");
			free_list_code(head_codeline);
			exit(1);
		}
		unpackline(new_line, line, new_label);
		if (line->islabel == 0)
		{
			head_codeline = add_codeline(head_codeline, line);
			free(new_label);
		}
		else if (line->islabel == 1)
		{
			head_lab = add_label(head_lab, new_label);
			head_codeline = add_codeline(head_codeline, line);
		}
	}
	fclose(asmb);
	address_locator(head_codeline, head_lab);
	address_allocator(head_codeline, head_lab);
	FILE* memin = fopen(argv[2], "w");
	if (memin == NULL)
	{
		printf("Error: opening %s failed\n", "C:\\Users\\benja\\OneDrive\\Desktop\\meminsp.txt");
		fclose(memin);
		free_list_code(head_codeline);
		exit(1);
	}
	/* Writing in updated_components.txt */


	//the below memory allocation is for 
	char** linesArray = (char**)malloc(sizeof(char*) * LINE_NUMBER);
	for (int i = 0; i < LINE_NUMBER; i++) {
		linesArray[i] = (char*)malloc(LINE_LENGTH);
	}
	initialize_array(linesArray);
	write_to_array(head_codeline, linesArray);
	write_to_file(memin, linesArray);

	fclose(memin);
	free_list_code(head_codeline);
	free_list_lab(head_lab);

	return 0;
}
