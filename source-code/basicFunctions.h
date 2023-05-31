#ifndef SOURCE_CODE_BASICFUNCTIONS_H
#define SOURCE_CODE_BASICFUNCTIONS_H

//
// Created by desmond on 19.05.2023.
//
#include "definitions.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

void printINBUFF(char *str) {
	printf("%-25sINBUFFER CURRENT INDEX = %-2d  INBUFF :  ", str, cix);
	for (int i = 0; inbuff[i] != '\0'; i++) {
		if (i < cix)
			printf(ANSI_COLOR_GREEN "%d|" ANSI_COLOR_RESET, (uint8_t) inbuff[i]);
		else
			printf("%d|", (uint8_t) inbuff[i]);
	}
	printf("\n");
}


void printOUTBUFF(char *str) {
	printf("%-25sOUTBUFFER CURRENT INDEX = %-2d OUTBUFF : \n", str, cox);
	for (int i = 0; i < cox ; i++) {
		printf(ANSI_COLOR_BLUE "\t%d\n" ANSI_COLOR_RESET, (uint8_t) outbuff[i]);
	}
}

void printSTMCUR(char *str) {
	printf("%-25s STMCUR : ", str);
	printf(ANSI_COLOR_RED "%d|%d|" ANSI_COLOR_RESET, (uint8_t) stmcur[0], (uint8_t) stmcur[1]);
	for (int i = 2; stmcur[i]  != '\0'; i++) {
		printf(ANSI_COLOR_RED "%d|" ANSI_COLOR_RESET, (uint8_t) stmcur[i]);
	}
	printf("\n");
}

void printSTATEMENTTABLE(char *str) {
	printf("%sSTATEMENT TABLE:\n", str);

	for (int i = 0; ; i++) {
		if (stmtab[i] == '\0' && stmtab[i + 1] == '\0') {
			break;
		}
		printf("%d|",  stmtab[i]);
		if (stmtab[i] == 22 ) {
			printf("\n");
		}
	}

	printf("\n");
}

void printStack(char *str, char *lbl) {
	printf(ANSI_COLOR_BR_RED"STACK %s %s:\n"ANSI_COLOR_RESET, str, lbl);
	printf("\n");
	for (int i = stklvl; i > 0; i -= 4) {
		printf("Stack level: %d  PUSHED PROGRAM COUNTER: %d PUSHED OUTBUFFER INDEX: %d PUSHED INBUFFER INDEX: "
		       "%d\n--------------------------\n", i, stack[i - 1], stack[i - 2], stack[i - 3]);
	}
	printf("\n");
}

int labelSearch(char *str) {                                         //finds index of str parameter in LABEL array
	for (int i = 0; i < 114; i++) {                                         //which keeps locations and names of labels
		if (strcmp(LABEL[i].name, str) == 0) {
			return i;
		}
	}
	return -1;
}

char *getFromTable(void *table) {
	char *temp = NULL;
	if (table == sntab) {                                             //if table is statement name table
		temp = ((struct table *) (table))[stenum].name;
	} else if (table == OPNTAB_STRING) {                               //if opntab string table
		temp = ((struct table *) (table))[stenum].val.str;
	} else if (table == vntab) {
		int stm = -1;
		int i = 0;
		int first = 0;
		while (stm != stenum) {
			if (vntab[i] & 0x80) {
				stm++;
				if (vntab[i + 1] != '\0')
					first = i + 1;
			}
			if (vntab[i] == '\0')
				return NULL;
			i++;
		}
		memcpy(tempArr, vntab + first, i - first);
		tempArr[i - first - 1] &= 0x7f;
		tempArr[i - first] = '\0';
		return tempArr;
	}
	return temp;
}

int search(void *table,
	   int SRCNXT) {                               //It is a deus ex machina searches tokens in different tables
	int size;                                                       //it is size of searching token
	int error;
	int situation = 0;
	char *temp;                                                     //it will be the token which will be searched
	if (!SRCNXT)                                                     //if srcnxt==1 it will not initialize stenum as -1 so the search function will continue to search from last index instead of stenum=0
		stenum = -1;

	while (1) {
		switch (situation) {                                        //gets new token from table
			case 0:
				stenum++;
				bufferIndex = cix;
				tableIndex = 0;
				if (getFromTable(table) == NULL)
					return 1;
				error = 0;                                           //error is 0 if all the char comparisons are true
			case 1:
				if (inbuff[bufferIndex] == '.') {                 //wild card check
					situation = 5;
					break;
				}
			case 2:

				temp = getFromTable(
					table);                           //it gets token from table and comparises buffer char and token char
				size = strlen(temp);
				if (inbuff[bufferIndex] == temp[tableIndex]) {        //if characters are matched
					situation = 3;                                    //goes to check next characters
					break;
				}
				error = 1;                                            //if characters arent matched makes error flag as 1
				// TODO: ADD BREAK HERE IF EVERYTHING WORKS WELL
			case 3:
				bufferIndex++;                                  //increments indexes to check next chars
				tableIndex++;
				if (tableIndex < size) {
					situation = 2;                                    //if size > table index search function goes to get new token from table
					break;
				}                                                   //if table index >= size of buffer token it is time to make decision whether buffer token and table token are matched
				if (error ==
				    0) {                                      //if error flag was not set program returns 0; 0 return means the token was found
					return 0;
				}
				situation = 0;                                        //else goes to get and check new token from table
		}
	}
}

int opnTabSearch(
	char *str) {                                        //searches str token in operator name table and returns index of the token if the token was found
	for (int i = 0; i < 55; i++) {
		if (strcmp(OPNTAB[i].name, str) == 0) {
			return i;
		}
	}
	return -1;
}

void setcode(char *a) {                                             //setcode function puts 1 byte to outbuff
	if (a == NULL)
		outbuff[cox++] = '\0';
	else { outbuff[cox++] = *a; }

	printOUTBUFF("SETCODE: ");
	if (cox == 0) { printf("line is too long"); }
}

short getlnum() {                                                   //calculates line number and puts to outbuff
	short lNum = 0;
	while (inbuff[cix] <= 57 && inbuff[cix] >= 48) {
		lNum *= 10;
		lNum += (short) (inbuff[cix++] - '0');
	}
	printINBUFF("GETLNUM: ");
	binint = lNum;
	// TODO: feed lNum to CVAFP
	// TODO: get the CVAFP out and feet to CVASC then return it
	char low = (char) (lNum & 0xff00);
	char high = (char) (lNum & 0x00ff);
	setcode(&high);
	setcode(&low);
	return lNum;
}


void skblank() {                                                     //skips all blanks in inbuff until a non blank character appear
	while (inbuff[cix] == ' ')
		cix++;
	printINBUFF("SKBLANK: ");
}

void getll() {
	linelength = stmcur[2] ;
}

void gnxtl(){
	stmcur += linelength;
}

// exit parameters:
// if the line number is found stmcur contains the address of it and returns 0
// if the line number is not found stmcur contains the address where a statement with that line number should be inserted and returns 1
int getstmt() {
	savcur = stmcur; // save the current address
	stmcur = stmtab; // set stmcur to indicate the top of the Statement Table

	while (1){
		if (tslnum == *stmcur){
			return 0;
		}else if (tslnum > *stmcur){

			return 1;
		}else{
			getll();
			gnxtl();
			continue;
		}
	}

}

// contract and contlow are practically same , it differs in atari because contract us used for big shifts
// and contlow used for small shifts (CONTract LOW)
void contract(char *stcur, int contsize) {
	int index = stcur - stmtab;
	char temp;

	// non null array size
	int stmtabsize = 0;
	for (; stmtab[stmtabsize] != '\0'; stmtabsize++) {}

	for (; stmtab[index] != '\0'; index++) {
		temp = stmtab[index];
		stmtab[index - contsize] = temp;
	}

	for (int i = stmtabsize - 1; i > stmtabsize - contsize; i--) {
		stmtab[i] = '\0';
	}
}

// expand and explow are practically same , it differs in atari because contract us used for big shifts
// and explow used for small shifts (EXPand LOW)
void expand(char *x, int y) {
	// get next line's pointer
	// start from end of the statement table and iterate to next line' pointer
	// for each iteration shift the current element to the right by expand amount
	// after shifting is done insert the new line to the statement table
	int index = x - stmtab;
	svesa = x;
	char temp;
	// non null array size
	int stmtabsize = 0;
	for (; stmtab[stmtabsize] != '\0'; stmtabsize++) {}

	for (int i = stmtabsize - 1; i >= index; i--) {
		temp = stmtab[i];
		stmtab[i + y] = temp;
	}
}

void syncon() { // uses hardware push
	char temp = cox;
//    push(cox);                 //azaltılacak alan
	temp = *(stmcur + temp);        // this line and line below acts as get next line
	stmcur = &temp;
	contract(stmcur, temp);
}

void stgo(int rega){
	printf("IN STGO\n");

	printSTATEMENTTABLE("STGO: ");

}

int tendst() {
	return (stmcur[1]);
}

void setln1(){
	linelength = stmcur[2];
	nxtstd = 3;
}

void execnl(){
	setln1();
}


void xend(){
//	syntax();
	return ;
}

void exeol() {
	if (stmcur[1] < 0) {
		printf("ready\n");
		return;
	} else {
		gnxtl(linelength);
		if (tendst()) {
			execnl();
		} else {
			xend();
		}
	}
}



void setline(){
	getstmt();
	setln1();
}

void execns(){
	if (nxtstd >= linelength ){
		exeol();

	}else{
		nxtstd = stmcur[nxtstd] ;
		int rega = stmcur[nxtstd+ 1];
		stindex = nxtstd+ 2;
		stgo(rega);
		execns();

	}
}


void execution(){
	execnl();
	execns();
}

#endif //SOURCE_CODE_BASICFUNCTIONS_H