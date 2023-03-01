#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basicFunctions.h"
#include "definitions.h"
#include "syntaxer.h"
#include "constantFunctions.h"
#include "variableFunctions.h"
#include "initializer.h"




int main(void){
    initializeTables();                                  //reads tables statement name table, operator token table, syntax table and
                                                        //creates program[] array what is syntax table
        printf("ready\n");    
    while(1) {                                          //program editor loop: takes input from and tokenizes
        char strEnd[2]={'\r','\0'};
        OPNTAB_STRING[6].val.str=strEnd;                //added because program doesnt read \r as char from opntab-string-values.txt
        gets(inbuff);                                   //gets input from user
        inbuff[strlen(inbuff)]='\r';
        cix = 0;                                        //initializes input buffer index as 0
        cox = 0;                                        //initializes output buffer index as 0
        printINBUFF("EDITOR INITIAL: ");
        printOUTBUFF("EDITOR INITIAL: ");
        DIRFLG = 0;                           //initializes direct statement flag as 0(direct statements execute program)
        skblank();                                      //skips blanks
        getlnum();                                      //gets line number from input and locates to outbuff's first 2 byte
        setcode(NULL);                               //leaves 1 byte space for line length after syntax operation editor will replace as line number
        if (binint < 0)                                 //binint was set in getlnum function. binint is default 8000 if user enter line number its set in getlnum
            DIRFLG = 1;
        skblank();
        stmstart=cix;                                   //statement start index
        if (inbuff[cix] == '\n') {                      //if program editor is at end of the line and direct statement(user enters blank line)
            if (DIRFLG == 1)                  //program editor goes to request new line
                continue;

        }
        stmlbd=cox;                                     //stmlbd keeps the index which will be determined as statement length
        setcode(NULL);                               //reserves 1 byte space for statement length
        skblank();
        search(sntab,0);                        //searches the next token in statement table. if finds keeps the statement's index as stenum
        cix=bufferIndex;                                //equalizes input buffer index to old buffer index + token size
        printINBUFF("EDITOR AFTER SEARCH: ");
        setcode(&stenum);                               //5. index of outbuff is become statement number
        skblank();
        if(synent()){                                   //calls the synaxer.
            fprintf(stderr,"error\n");           //if an error occures synent returns 1
        }
            outbuff[2]=cox;                             //sets line length
            outbuff[stmlbd]=cox;                        //sets statement length

        printINBUFF("EDITOR - END OF EDITOR: ");
        printOUTBUFF("EDITOR - END OF EDITOR: ");
        printf("ready\n");                      //writes to user ready to get a new line
    }
}

/*
 * STATEMENT TABLE FORMAT
 *    [][]          []            []                      []            [...]   []         []                      []            [...]   []    []
 * linenumber | linelength | statement length | statement name token | tokens | eos | statement length | statement name token | tokens | eos | eol
 */
void synok(){

    char temp = cox;
    outbuff[stmlbd]=cox;
    if(inbuff[cix-1]=='\r'){ // SYN4
        outbuff[2]=cox;
       int linel;
        if(getstmt()) { // statement found
            linel = 0;
        }
        linel = getll();
        if (cox == linel){
            //synin
        }else if (cox > linel){
            // syncon
        }


            temp = linelength-cox-2;
            temp ^= 0xff;

            explow(stmcur, temp);
            svesa= stmcur;
            if(svesa!='\-1')
                synin();

    }
    else{ // syn1
        stmlbd = cox;
        setcode(cox);

        skblank();
        search(sntab, 2); // TODO KEEP AN EYE ON THIS
        cix = bufferIndex;
        setcode(stenum);
        skblank();
        if(!synent()){
           synok();
        }
        if(inbuff[maxcix] == '\r'){
           inbuff[maxcix+1] =   inbuff[maxcix] ;
           inbuff[maxcix]  = 0x20 | 0x80;
        }else{
            inbuff[maxcix] = inbuff[maxcix] | 0x80;
        }
        DIRFLG = DIRFLG | 0x40; // ??????????????????
        cix = stmstart;
        stmlbd = 3;
        cox = 4;

        while(inbuff[cix] != '\r') {
            setcode(0x37); //0x37 is CERR
            cix++;
        }
        setcode(inbuff[cix]);

    }

    // execute();
}
