//
// Created by desmond on 18.05.2023.
//

#ifndef SOURCE_CODE_SYNTAXER_H
#define SOURCE_CODE_SYNTAXER_H

#include <ctype.h>


void sdel();
void synok();

void syn1();
void syn3();
void xdata();
void syn4();
void syn5();
void syn5a();
void syn6();
void syn0();
void synin();
void syn7();
void syn8();
void syn9();
void writeOutbuffToSTMTAB();
char* labelName;
int nxsc() {                                                                //gets new code program array
    return program[++cpc];
}


int erem(){
    return 0;
}

int edata(){
    return 0;
}

int eif(){
    return 0;
}

int echng(){                                                                //changes previous outbuff token            //the tokens are same characters but have different meanings
    outbuff[cox-1] = (char)(program[++cpc]&0x00ff);
    printOUTBUFF("CHNG : ");
    return(0);
}

char* getLabel(long location){                                              //it is a debug function which indicates which label is executed
    int i;
    for(i=0;i<120;i++){
        if(LABEL[i].location==location)
            break;
    }
    return LABEL[i].name;

}

int fail(){                                                                 //if a function which is called from synent fails(returns 1) then fail function is called
    while(1){                                                               //gets next code from program array
        code=nxsc();
        while (code<2 || code >3 ){                                         //program counter is incremented until it will program[cpc] = 2 or 3; 2 means or 3 means return
            if (code == 0 ){                                                //if code==0 it means call instruction and it is followed by address so program counter incremented to skip address
                cpc++;
            }
            code=nxsc();
        }
        if(code==3){                                                        //ret instruction
            if(stklvl==4){                                                  //stack is empty so error was occured then returns error
                return 1;
            }
            cpc=stack[stklvl-1];                                            //pulls from stack previous program counter
            stklvl-=4;
            labelName=getLabel(stack[stklvl]);
            printStack( ANSI_COLOR_BR_YELLOW"AFTER RET",labelName);
        }
        else{
            if(cix>maxcix){                                                 //line is too long
                maxcix = cix;
            }
            cix = stack[stklvl-3];                                          //pulls from stack input buffer index and output buffer index
            printINBUFF("FAIL OR: ");
            cox = stack[stklvl-2];                                          //syntaxer will try next OR or RET
            printOUTBUFF("FAIL OR: ");
            return 0;                                                       //return 0 means no error
        }

    }
}



int synent() {

    cpc =  LABEL[sntab[stenum].val.num].location;                           //gets the statement's syntax block's address -1
    labelName = getLabel(cpc);
    stklvl = 0;                                                             //initializes stack

    stack[1]=cix;                                                           //pushes buffer indexes and program counter to stack to error examination
    stack[2]=cox;
    stack[3]=cpc;   //record the present address.
    stklvl+=4;
    printStack("AFTER SYNENT INITIALIZATION LABEL: ",labelName);

    while (1) {
        code = nxsc();                                                      //gets instruction
        if (code == 0x0000) {                                               //call instruction
            code = nxsc();
            stack[stklvl]=code;                                                //gets address to jump
            stack[stklvl + 1] = cix;                                        //pushes variables to stack before call
            stack[stklvl + 2] = cox;
            stack[stklvl + 3] = cpc;
            cpc = code;
            //cpc points to called label now
            labelName = getLabel(code);
            printf(ANSI_COLOR_YELLOW"SYNENT -CALL cpc=%d\n"ANSI_COLOR_RESET,cpc);
            printf(ANSI_COLOR_BR_RED"SYNENT-CALL LABEL=%s\n"ANSI_COLOR_RESET,getLabel(code));
            stklvl += 4;                                                    //stack level is incremented
            printStack(ANSI_COLOR_BR_MAGENTA"AFTER CALL LABEL: ",labelName);

        }
        else if (code == 0x0001) {                                          //echng instruction which changes previous token depending on token meaning
            if(echng())
                if(fail()){                                                 //if echng function returns error then synent calls fail()
                    return 1;                                               //if fail() also returns error than synent returns error too.
                }
        }
        else if (code == 0x0004) {                                          //tests whether inbuff token is numeric constant
            if(tncon())
                if(fail()){
                    return 1;
                }
        }
        else if (code == 0x0005) {                                          //tests whether inbuff token is numeric variable
            if(tnvar())
                if(fail()){
                    return 1;
                }
        }
        else if (code == 0x0006) {                                          //tests whether inbuff token is string constant
            if(tscon())
                if(fail()){
                    return 1;
                }
        }
        else if (code == 0x0007) {                                           //tests whether inbuff token is string variable
            if(tsvar())
                if(fail()){
                    return 1;
                }
        }
        else if (code == 0x000a) {
            if (eif())
                if(fail()){
                    return 1;
                }
        }
        else if (code==0x0008) {
            if (edata())
                if(fail()){
                    return 1;
                }
        }
        else if (code==0x0009) {
            if (erem())
                if(fail()){
                    return 1;
                }
        }
        else if (code ==2 || code ==3 ) { //or veya return
            if (stklvl == 4) {
                printStack(ANSI_COLOR_BRIGHT_GREEN"STACK LEVEL = 4 SYNENT RETURNS PASS SUCCESSFULLY"ANSI_COLOR_RESET,"");
                return 0; }
            cpc = stack[stklvl - 1];
            stklvl -= 4;
            labelName=getLabel(stack[stklvl]);
            code==2?printStack("INSIDE SYNENT AFTER OR","") : printStack(ANSI_COLOR_BR_YELLOW"INSIDE SYNENT AFTER RET",labelName);
            if (stklvl < 0)
                fail();
        }
        else if ((code>=16)&&(code<=127)) {                                 //current instruction is constant
            if (srcont())                                                   //checks the constant whether in operator table
                if(fail()){
                    return 1;
                }
        }
        else
            printf("unrecognized code in syntax table\n");
    }
}

void synok() {
    REGISTER_A = cox;                // lda cox
    REGISTER_Y = stmlbd;             // ldy stmlbd
    outbuff[REGISTER_Y] = REGISTER_A;       // sta [outbuff], Y
    printOUTBUFF("SYNOK: ");
    REGISTER_Y = cix;                     // ldy cix
    REGISTER_Y--;                         // DEY
    REGISTER_A = inbuff[REGISTER_Y];         // lda [inbuff], Y

    if (REGISTER_A != '\r') { // CMP #CR
        syn1();             // BNE :SYN1
    } else {
      syn4();
    }
}

void syntax() {
	printf("ready\n");
    char strEnd[2] = {'\r', '\0'};
    OPNTAB_STRING[6].val.str = strEnd;                //added because program doesnt read \r as char from opntab-string-values.txt
    gets(inbuff);                                   //gets input from user

    inbuff[strlen(inbuff)] = '\r';
    REGISTER_A = 0; // LDA #0
    cix = REGISTER_A; // STA CIX                                 //initializes input buffer index as 0
    maxcix = REGISTER_A; // STA maxcix
    cox = REGISTER_A; // STA cox                                        //initializes output buffer index as 0
    printINBUFF("EDITOR INITIAL: ");
    printOUTBUFF("EDITOR INITIAL: ");
    DIRFLG = 0;  // STA DIRFLG                         //initializes direct statement flag as 0(direct statements execnl program)
    svontx = REGISTER_A;
    svontc = REGISTER_A;
    svvvte = REGISTER_A;
    // previous authors ignored from [A086 - A08C]
    skblank();  // JSR SKBLANK                                      //skips blanks
    getlnum();  // JSR :GETLNUM                                    //gets line number from input and locates to outbuff's first 2 byte
    setcode(NULL);                               //leaves 1 byte space for line length after syntax operation editor will replace as line number
    REGISTER_A = binint + 1; // LDA BININT+1
    if (binint <0) {                              //binint wages set in getlnum function. binint is default 8000 if user enter line number its set in getlnum
        DIRFLG = REGISTER_A; // STA DIRFLG
    }
    syn0();     // BPL SYN0
    printINBUFF("EDITOR - END OF EDITOR: ");
    printOUTBUFF("EDITOR - END OF EDITOR: ");
}

void syn0() {
    skblank();      // JSR SKBLANKS
    REGISTER_Y = cix; // LDY CIX
    stmstart = REGISTER_Y;              // STR STMSTRT    //statement start index
    REGISTER_A = inbuff[REGISTER_Y]; // LDA [INBUFF], Y
    if (REGISTER_A =='\r') {  // CMP #CR          //if program editor is at end of the line and direct statement(user enters blank line)
        if (DIRFLG == 1) {   //  BIT DIRFLG        //program editor goes to request new line
            syntax();   // BMI SYNTAX
        }else{
            sdel();     // JMP SDEL
        }
    }else{          // BNE SYN1;
        syn1();
    }
}

// a.k.a synl
void syn1() {
    // synl a.k.a xif
    stmlbd = cox;
    setcode(NULL);   // JSR :SETCODE
    skblank();                // JSR SKBLANK
    search(&sntab, 0); // JSR SEARCH
    cix = bufferIndex;                   // STX
    printINBUFF("EDITOR AFTER SEARCH: ");
    REGISTER_A = stenum;                // LDA STENUM
    setcode(&REGISTER_A);            // LDA STENUM
    skblank();                         // JSR SKBLANK


    if (!synent()) {                     // JSR SYNENT
	    synok();                            // BCC SYNOK
    }else{
	    fprintf(stderr, "error\n");
    }

    REGISTER_Y = maxcix;             // LDY MAXCIX
    REGISTER_A = inbuff[REGISTER_Y];         // LDA [INBUFF], Y
    if (REGISTER_A == '\r') {              // CMP #CR ;
	    REGISTER_Y++;                      // INY
	    inbuff[REGISTER_Y] = REGISTER_A;     // LDA [INBUFF], Y
	    REGISTER_Y--;                      // DEY
	    REGISTER_A = 0x20;                  // LDA #$20
    }
//    syn3a();
	syn3();
}

void syn3a() {
    REGISTER_A = REGISTER_A | 0x80;        // ORA #$80 // TODO: ask about this one
    inbuff[REGISTER_Y] = REGISTER_A;       // STA [INBUFF], Y
    REGISTER_A = 0x40;                // LDA #$40
    REGISTER_A = REGISTER_A | DIRFLG;
    DIRFLG = REGISTER_A;
//    REGISTER_Y = stmstart;            // LDY STMSTRT
//    cix = REGISTER_Y;                  // STY CIX
    REGISTER_X = 3;                    // LDX #3
    stmlbd = REGISTER_X;              // STX STMLBD
    REGISTER_X++;                     // INX
    cox = REGISTER_X;                 // STX COX
    syn3();
}

void syn4(){
	REGISTER_Y = 2;
	REGISTER_A = cox;
	outbuff[REGISTER_Y] = REGISTER_A; // sets line length to outbuff
	printOUTBUFF("IN SYN4");
	// TODO: ALL GOOD HERE
	syn5();
}

void syn5(){
	int tempCarry;

	if (!(DIRFLG)){

		tslnum = outbuff[0]; // set tslnum if line number given before calling the getstmt
	}else{
		tslnum = 800; // if its a direct statement the line number is 800
	}

	tempCarry = getstmt();
	REGISTER_A = 0;
	if (tempCarry){
		syn6();
	}else{
		syn5a();
	}

}

void syn5a(){
	getll();
	syn6();
}

void syn6(){
	// carry is one
	REGISTER_A = REGISTER_A - cox ;
	if (REGISTER_A == 0){
		synin();
	}else if (REGISTER_A > 0){
		syncon();
	}else{
		REGISTER_A ^= 0xff;
		REGISTER_Y = REGISTER_A;
		REGISTER_Y++;
		REGISTER_X = *stmcur;
		expand(stmcur, REGISTER_A);
		REGISTER_A = *svesa;
//		*stmcur = REGISTER_A;
		if (svesa+1  != '\0') {
			synin();
		}
	}
}

void synin() {

	REGISTER_Y = cox;
	syn7();

}

void syn7(){
	while(REGISTER_Y > 0){
		REGISTER_Y--;
		REGISTER_A = outbuff[REGISTER_Y];
		stmcur[REGISTER_Y] = REGISTER_A;
	}
	printSTATEMENTTABLE("");
	syn8();

}



void syn8() {

	if (!(DIRFLG)){
		syntax();
	}
	execution();

}
// TODO : IMPLEMENT SDEL
void sdel(){
    if(getstmt()){  // JSR GETSTMT
	    syntax();
    }
    getll();        // JSR GETLL
    REGISTER_A = linelength;
    char tempA = REGISTER_A;

}

void xdata(){
	REGISTER_Y = cix;
	REGISTER_A = inbuff[REGISTER_Y];
	cix++;
	if (REGISTER_A != '\r'){
		syn3();
	}else{
		setcode(&REGISTER_A);
		synok();
	}
}
void syn3(){
	setcode(&REGISTER_A);
	xdata();
}

#endif //SOURCE_CODE_SYNTAXER_H
