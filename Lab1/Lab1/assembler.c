//
//  assembler.c
//  
//
//  Created by Bryan Wang on 1/28/16.
//
//
#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

#define MAX_LINE_LENGTH 255
#define NUM_OPCODE 28
#define NUM_PSEUDO_OP 3
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255


//OK: One line read finished
//DONE: Whole file read finished
//EMPTY_LINE:
enum{DONE, OK, EMPTY_LINE};

static char *  OPCODE[NUM_OPCODE] = {"add", "and","br","brn","brz","brp","brzp","brnp","brnz","brnzp","halt", "jmp","jsr", "jsrr", "ldb", "ldw", "lea", "nop", "not", "ret", "rti", "lshf", "rshfl", "rshfa", "stb", "stw", "trap", "xor"};
static char * PSEUDO_OP[NUM_PSEUDO_OP] = {".orig", ".fill", ".end"};

struct label_addr_table{
	char label[MAX_LABEL_LEN+1]; //Label name
	int addr; //instruction count
};

//***********isOpcode*****************
// Check whether the string is valid OpCode, return -1 if not, return position in the list if it is.
//*************************************************
int isOpcode(char * const ptr){
	if (ptr) {
		for (int i = 0; i < NUM_OPCODE; i++) {
			if (strcmp(ptr, OPCODE[i]) == 0) {
				return i;
			}
		}
	}
	return -1;
}

//***********isPseudoOp*****************
// Check whether the string is valid PseudoOpCode, return -1 if not, return position in the list if it is.
//*************************************************
int isPseudoOp(char * const ptr){
	if (ptr) {
		for (int i = 0; i < NUM_PSEUDO_OP; i++) {
			if (strcmp(ptr, PSEUDO_OP[i]) == 0) {
				return i;
			}
		}
	}
	return -1;
	
}

//***********toNum*****************
// Convert string of # or x number into int
//*************************************************
int toNum( char * pStr ){
	char * t_ptr;
	char * orig_pStr;
	int t_length,k;
	int lNum, lNeg = 0;
	long int lNumLong;
	
	orig_pStr = pStr;
	if( *pStr == '#' ) /* decimal */
	{
		pStr++;
		if( *pStr == '-') /* dec is negative */
		{
			lNeg = 1;
			pStr++;
		}
		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for(k = 0;k < t_length;k++)
		{
			if (!isdigit(*t_ptr))
			{
				printf("Error: invalid decimal operand, %s\n",orig_pStr);
				exit(4);
			}
			t_ptr++;
		}
		lNum = atoi(pStr);
		if (lNeg)
			lNum = -lNum;
			return lNum;
			}
		else if( *pStr == 'x' ) /* hex */
		{
			pStr++;
			if( *pStr == '-') /* hex is negative */
		{
			lNeg = 1;
			pStr++;
		}
		t_ptr = pStr;
		t_length = strlen(t_ptr);
		for(k = 0;k < t_length;k++)
		{
			if (!isxdigit(*t_ptr))
			{
				printf("Error: invalid hex operand, %s\n",orig_pStr);
				exit(4);
			}
			t_ptr++;
		}
		lNumLong = strtol(pStr, NULL, 16); /* convert hex string into integer */
		lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
		if( lNeg )
			lNum = -lNum;
			return lNum;
			}
		else
		{
			printf( "Error: invalid operand, %s\n", orig_pStr);
			exit(4);
			/*This has been changed from error code 3 to error code 4, see clarification 12 */
		}
}

//***********check_label*****************
// Check whether the label name is legal
//*************************************************
void check_label(char * label){
	if(*label == '\0') return;
	if (label[0] == 'x' || (label[0] >= '0'&& label[0] <= '9')) {
		printf("Error: Labels can't start with 'x' or numbers\n");//Error 4
	}
	if(isOpcode(label) > -1 || isPseudoOp(label) > -1){
		printf("Error: Label name can't contain opcode/pseudo op\n"); //Error 4
	}
	if(strcmp(label, "in") == 0 || strcmp(label, "out") == 0 || strcmp(label, "getc") == 0 ||strcmp(label, "puts") == 0){
		printf("Error: Label name can't contain in/out/getc/puts\n"); //Error 4
	}
	for (int i = 0;i < strlen(label);i++){
		if (isalnum(label[i]) == 0) { //isalnum == 0 -> not a alphanumeric value
			printf("Error: Label name can only contain alphanumeric chars\n"); //Error 4
		}
	}
}

//***********read_reg*****************
// Use the reg string pointer to extract register number and return it. (R3 -> 3)
//*************************************************
int read_reg(char * reg){
	int reg_num = atoi(&reg[1]);
	if (reg[0] != 'r') {
		printf("Error: Wrong Register name");
	}
	if (reg[2] != '\0' || reg_num > 7) {
		printf("Error: Reg too large");
	}
	return reg_num;
}

//***********check_word_align*****************
// Check whether the address is word aligned. Return 0 if aligned
//*************************************************
int check_word_align(int num){// 1-> odd, not align
	return num % 2;
}

//***********check_8bit_unsigned*****************
// Check whether the number is in the range of 8 bit unsigned int, for trap vector
//*************************************************
void check_8bit(int num){
	if (num > 255 || num < 0) {
		printf("Error: 8 bit unsigned number overflow\n");
	}
}

//***********check_4bit_unsigned*****************
// Check whether the number is in the range of 4 bit unsigned int
//*************************************************
void check_4bit(int num){
	if (num > 15 || num < 0) {
		printf("Error: 4 bit unsigned number overflow\n");
	}
}

//***********check_5bit*****************
// Check whether the number is in the range of 5 bit signed int
//*************************************************
void check_5bit(int num){
	if (num > 16 || num <= -16) {
		printf("Error: 5 bit number overflow\n");
	}
}

//***********check_6bit*****************
// Check whether the number is in the range of 6 bit signed int
//*************************************************
void check_6bit(int num){
	if (num > 32 || num <= -32) {
		printf("Error: 6 bit number overflow\n");
	}
}

//***********check_9bit*****************
// Check whether the number is in the range of 9 bit signed int
//*************************************************
void check_9bit(int num){
	if (num > 256 || num <= -256) {
		printf("Error: 9 bit number overflow\n");
	}
}

//***********find_table***************************
// Find the lable in the table, geting its inst. count in order to calculate PC_offset. Return PC_offset
//*************************************************
int find_label(struct label_addr_table * table, char * label, int current_inst, int num_label){
	int label_inst = 0;
	for (int i = 0; i < num_label; i++) {
		if (strcmp(table[i].label, label) == 0) {
			label_inst = table[i].addr;
		}
	}
	if (label_inst == 0) {
		printf("Error: Label %s can't find.\n", label);
	}
	printf("Origin %d, current %d, instruction \"%s\"\n",label_inst,current_inst,label);
	return (current_inst - label_inst) > 0 ? current_inst - label_inst - 1 : current_inst - label_inst + 1;
}

//***********check_label_duplicate*****************
// Check whether the label is in previous label definitions
//*************************************************
void check_label_duplicate(struct label_addr_table * table, char * label, int num_label){
	for (int i = 0; i < num_label; i++) {
		if(strcmp(table[i].label, label) == 0){
			printf("Error: label duplicate in the table.\n");
		}
	}
}

//***********readAndParse*****************
// Read a line of intruction and split the line into strings of data, return DONE, OK, EMPTY_LINE
//*************************************************
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
             ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)//Pass by Reference, so it uses char **
{
    char * lRet, * lPtr;
    int i;
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
        return( DONE ); //Failed to read a line
	
	for( i = 0; i < strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );
        /* convert entire line to lowercase */
    
	*pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);
        /* ignore the comments */
	lPtr = pLine;
	while( *lPtr != ';' && *lPtr != '\0' && *lPtr != '\n' )
		lPtr++;
    
	*lPtr = '\0';
	if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
		return( EMPTY_LINE );

	if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
		{
			*pLabel = lPtr;
			if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
		}
    
	*pOpcode = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	*pArg1 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	*pArg2 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	*pArg3 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	*pArg4 = lPtr;
	return( OK );
}



int main(int argc, char* argv[]) {
	
    //char *prgName = NULL;
    //char *oFileName = NULL;
    FILE* infile = NULL;
    //FILE* outfile = NULL;
	
	char lLine[MAX_LINE_LENGTH+1], *lLable, *Opcode, *lArg1, *lArg2, *lArg3, *lArg4;
	int lRet;
	lLable = Opcode = lArg1 = lArg2 = lArg3 = lArg4 = NULL;
	
    //prgName = argv[0];
    //oFileName = argv[2];
	
	
    infile = fopen("/Users/bryanwang/Desktop/Computer Architecture/Lab1/Lab1/Lab1/inst.txt", "r");
	
	uint16_t origin_mem_addr = 0;
	
	struct label_addr_table table[MAX_SYMBOLS] = {0};
	
	int inst_count = 0;
	int label_count = 0;
	
	
	//Read instructions line by line 1st Round
	//Bond label to specific address(instruction count)
	do{
		lRet = readAndParse(infile, lLine, &lLable, &Opcode, &lArg1, &lArg2, &lArg3, &lArg4);
		if(lRet != DONE && lRet != EMPTY_LINE){
			inst_count++;
			printf("NUM of Label: %d\n",label_count);
			//printf("One run\n");
			if (lLable) printf("Label : %s\n",lLable);
			if (Opcode) printf("OP : %s\n",Opcode);
			if (lArg1) printf("Arg 1 : %s\n",lArg1);
			if (lArg2) printf("Arg 2 : %s\n",lArg2);
			if (lArg3) printf("Arg 3 : %s\n",lArg3);
			if (lArg4) printf("Arg 4 : %s\n\n",lArg4);
			//printf("PSEUDO_OP is %d\n",(lLable == NULL) );
			check_label(lLable);
			if (*lLable == '\0' && isPseudoOp(Opcode) == 0 ) {
				if (origin_mem_addr != 0 || !*lArg1 || *lArg2 || *lArg3 || *lArg4) {
					printf("Error: .Orig Syntax \n");
					exit(2);
				}
				else {
					int tmpaddr = toNum(lArg1);
					if (tmpaddr > UINT16_MAX) {
						printf("Error: Address is Out of 16 bit Memory. \n");
					}
					if(check_word_align(tmpaddr)){
						printf("Error: Not word alignment");
					}
					else{
						origin_mem_addr = tmpaddr;
						printf("Start Addr : 0x%X\n",origin_mem_addr);
					}
				}
			}
			if (*lLable != '\0' && isPseudoOp(Opcode) == 2 ) {
				if ( *lArg1 == '\0' || *lArg2 == '\0' || *lArg3 == '\0' || *lArg4 == '\0')
					printf("Error END Syntax \n");
				else{
					lRet = DONE;
					printf(".END command = End of Program. Exiting...\n");
				}
			}
			if(*lLable){
				check_label_duplicate(table,lLable,label_count);
				strcpy(table[label_count].label, lLable);
				table[label_count].addr = inst_count;
				(label_count) += 1;
			}
		}
	}
	while (lRet != DONE);
	
	rewind(infile);
	printf("Starting 2nd passing\n");
	inst_count = 0;
	
	//Begin 2nd pass of the file, assume error free, otherwise exit by previous pass
	do{
		lRet = readAndParse(infile, lLine, &lLable, &Opcode, &lArg1, &lArg2, &lArg3, &lArg4);
		inst_count++;
		if(lRet != DONE && lRet != EMPTY_LINE){
			//.ORIG
			if(isPseudoOp(Opcode) == 0){
				printf("Ox%x\n",origin_mem_addr);
			}
			//.FILL
			else if (isPseudoOp(Opcode) == 1) {
				printf("0x%x\n",toNum(lArg1));
			}
			//.END
			else if (isPseudoOp(Opcode) == 2) {
				lRet = DONE;
			}
			else if(strcmp(Opcode, "add") == 0 || strcmp(Opcode, "and") == 0 || strcmp(Opcode, "xor") == 0){
				if (*lArg1 != '\0' && *lArg2 != '\0' && *lArg3 != '\0') {
					int dr = read_reg(lArg1);
					int sr1 = read_reg(lArg2);
					int sr2;
					if (lArg3[0] == 'x' || lArg3[0] == '#') {
						sr2 = toNum(lArg3);
						check_5bit(sr2);
						if (strcmp(Opcode, "add") == 0) {
							printf("0x%x\n",0x1000 + (dr<<9) + (sr1<<6) + sr2 + 32);
						}
						else if (strcmp(Opcode, "and") == 0){
							printf("0x%x\n",0x5000 + (dr<<9) + (sr1<<6) + sr2 + 32);
						}
						else if (strcmp(Opcode, "xor") == 0){
							printf("0x%x\n",0x9000 + (dr<<9) + (sr1<<6) + sr2 + 32);
						}
						
					}
					else{
						sr2 = read_reg(lArg3);
						if (strcmp(Opcode, "add") == 0) {
							printf("0x%x\n",0x1000 + (dr<<9) + (sr1<<6) + sr2);
						}
						else if (strcmp(Opcode, "and") == 0){
							printf("0x%x\n",0x5000 + (dr<<9) + (sr1<<6) + sr2);
						}
						else if (strcmp(Opcode, "xor") == 0){
							printf("0x%x\n",0x9000 + (dr<<9) + (sr1<<6) + sr2);
						}
					}
				}
				else
					printf("Error: Wrong Syntax for and/and/xor");
			}
			
			else if(strncmp(Opcode, "br", 2) == 0){
				if (*lArg1 != '\0' && *lArg2 == '\0'&& *lArg3 == '\0'&& *lArg4 == '\0') {
					int offset = 0;
					offset = find_label(table, lArg1, inst_count, label_count);
					switch (strlen(Opcode)) {
						case 2:
							printf("0x%x\n",offset);
							break;
						case 3:
							if (Opcode[2] == 'n') {
								printf("0x%x\n",offset + 0x8000);
							}
							else if (Opcode[2] == 'z') {
								printf("0x%x\n",offset + 0x4000);
							}
							else if (Opcode[2] == 'p') {
								printf("0x%x\n",offset + 0x2000);
							}
							break;
						case 4:
							if (Opcode[2] == 'n' && Opcode[3] == 'p') {
								printf("0x%x\n",offset + 0xA000);
							}
							else if (Opcode[2] == 'z' && Opcode[3] == 'p') {
								printf("0x%x\n",offset + 0x6000);
							}
							else if (Opcode[2] == 'n'&& Opcode[3] == 'z') {
								printf("0x%x\n",offset + 0xC000);
							}
							break;
						case 5:
							printf("0x%x\n",offset + 0xE000);
							break;
						default:
							printf("Error: OPCODE Wrong\n");
							break;
					}
				}
				else
					printf("Error: Wrong Syntax for br");
			}
			else if(strcmp(Opcode, "jmp") == 0){
				if (*lArg1 != '\0' && *lArg2 == '\0'&& *lArg3 == '\0'&& *lArg4 == '\0'){
					int baser = read_reg(lArg1);
					printf("0x%x\n",(baser << 6) + 0xC000);
				}
				else
					printf("Error: Wrong Syntax for jmp");
				
			}
			else if(strcmp(Opcode, "ret") == 0){
				if (*lArg1 == '\0' && *lArg2 == '\0'&& *lArg3 == '\0'&& *lArg4 == '\0'){
					printf("0xC1C0\n");
				}
				else
					printf("Error: Wrong Syntax for ret");
				
			}
			else if(strncmp(Opcode, "jsr",3) == 0){
				if (*lArg1 != '\0' && *lArg2 == '\0'&& *lArg3 == '\0'&& *lArg4 == '\0'){
					if (Opcode[3]  == '\0'){
						int offset = find_label(table, lArg1, inst_count, label_count);
						printf("0x%x\n",offset + 0x4800);
					}
					else{
						int baser = read_reg(lArg1);
						printf("0x%x\n",(baser << 6) + 0x4000);
					}
				}
				else
					printf("Error: Wrong Syntax for jsr/jsrr");
				
			}
			else if(strncmp(Opcode, "ld" , 2) == 0){
				if (*lArg1 != '\0' && *lArg2 != '\0'&& *lArg3 != '\0'&& *lArg4 == '\0'){
					int dr = read_reg(lArg1);
					int baser = read_reg(lArg2);
					int offset6 = toNum(lArg3);
					check_6bit(offset6);
					if (Opcode[2] == 'b') {
						printf("0x%x\n",(dr << 9) + (baser << 6) + 0x2000 + offset6);
					}
					else if(Opcode[2] == 'w'){
						printf("0x%x\n",(dr << 9) + (baser << 6) + 0x6000 + offset6);
					}
					
				}
				else
					printf("Error: Wrong Syntax for ldb/ldw");
				
			}
			else if(strcmp(Opcode, "lea") == 0){
				if (*lArg1 != '\0' && *lArg2 != '\0'&& *lArg3 == '\0'&& *lArg4 == '\0'){
					int dr = read_reg(lArg1);
					int offset9 = find_label(table, lArg2, inst_count, label_count);;
					check_9bit(offset9);
					printf("0x%x\n",(dr << 9) + 0xE000 + offset9);
				}
				else
					printf("Error: Wrong Syntax for lea");
				
			}
			else if(strcmp(Opcode, "not") == 0){
				if (*lArg1 != '\0' && *lArg2 != '\0'&& *lArg3 == '\0'&& *lArg4 == '\0'){
					int dr = read_reg(lArg1);
					int sr = read_reg(lArg2);
					printf("0x%x\n",(dr << 9) + (sr << 6) + 0x903F);
				}
				else
					printf("Error: Wrong Syntax for not");
				
			}
			else if(strcmp(Opcode, "rti") == 0){
				if (*lArg1 == '\0' && *lArg2 == '\0'&& *lArg3 == '\0'&& *lArg4 == '\0'){
					printf("0x8000\n");
				}
				else
					printf("Error: Wrong Syntax for rti");
				
			}
			else if(strcmp(Opcode, "lshf") == 0){
				if (*lArg1 != '\0' && *lArg2 != '\0'&& *lArg3 != '\0'&& *lArg4 == '\0'){
					int dr = read_reg(lArg1);
					int sr = read_reg(lArg2);
					int amount4 = toNum(lArg3);
					check_4bit(amount4);
					printf("0x%x\n",(dr << 9) + (sr << 6) + 0xD000 + amount4);
					
				}
				else
					printf("Error: Wrong Syntax for lshf");
				
			}
			else if(strncmp(Opcode, "rshf" , 4) == 0){
				if (*lArg1 != '\0' && *lArg2 != '\0'&& *lArg3 != '\0'&& *lArg4 == '\0'){
					int dr = read_reg(lArg1);
					int sr = read_reg(lArg2);
					int amount4 = toNum(lArg3);
					check_4bit(amount4);
					if (Opcode[4] == 'l') {
						printf("0x%x\n",(dr << 9) + (sr << 6) + 0xD010 + amount4);
					}
					else if(Opcode[4] == 'a'){
						printf("0x%x\n",(dr << 9) + (sr << 6) + 0xD030 + amount4);
					}
				}
				else
					printf("Error: Wrong Syntax for rshfa/rshfl");
				
			}
			else if(strncmp(Opcode, "st" , 2) == 0){
				if (*lArg1 != '\0' && *lArg2 != '\0'&& *lArg3 != '\0'&& *lArg4 == '\0'){
					int sr = read_reg(lArg1);
					int baser = read_reg(lArg2);
					int offset6 = toNum(lArg3);
					check_6bit(offset6);
					if (Opcode[2] == 'b') {
						printf("0x%x\n",(sr << 9) + (baser << 6) + 0x3000 + offset6);
					}
					else if(Opcode[2] == 'w'){
						printf("0x%x\n",(sr << 9) + (baser << 6) + 0x7000 + offset6);
					}
					
				}
				else
					printf("Error: Wrong Syntax for stb/stw");
				
			}
			else if(strcmp(Opcode, "trap") == 0){
				if (*lArg1 != '\0' && *lArg2 == '\0'&& *lArg3 == '\0'&& *lArg4 == '\0'){
					int trap_vector8 = toNum(lArg1);
					check_8bit(trap_vector8);
					printf("0x%x\n",0xF000 + trap_vector8);
				}
				else
					printf("Error: Wrong Syntax for trap");
				
			}
			
		
		}
	}
	while (lRet != DONE);
	
	
    
    fclose(infile);
    //fclose(outfile);
	 
}