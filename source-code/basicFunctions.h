#include "definitions.h"
#include <stdio.h>
#include <inttypes.h>
void printINBUFF(char* str){
    printf("%-25sINBUFFER CURRENT INDEX = %-2d  INBUFF :  ",str,cix);
    for(int i=0;i<30;i++){
        if(i<cix)
            printf(ANSI_COLOR_GREEN "%d|" ANSI_COLOR_RESET,(uint8_t)inbuff[i]);
        else
            printf("%d|",(uint8_t)inbuff[i]);
    }
    printf("\n");
}
void printOUTBUFF(char* str){
    printf("%-25sOUTBUFFER CURRENT INDEX = %-2d OUTBUFF : \n",str,cox);
    for(int i=0;i<cox;i++){
            printf(ANSI_COLOR_BLUE "\t%d\n" ANSI_COLOR_RESET,(uint8_t)outbuff[i]);
    }
}

void printStack(char* str,char*lbl){
    printf(ANSI_COLOR_BR_RED"STACK %s %s:\n"ANSI_COLOR_RESET,str,lbl);
    for(int i=stklvl ;i>0;i-=4 ){
        printf("Stack level: %d  PUSHED PROGRAM COUNTER: %d PUSHED OUTBUFFER INDEX: %d PUSHED INBUFFER INDEX: "
               "%d\n--------------------------\n",i,stack[i-1],stack[i-2],stack[i-3]);
    }
    printf("\n");
}


int labelSearch(char* str){                                         //finds index of str parameter in LABEL array
    for(int i=0;i<114;i++){                                         //which keeps locations and names of labels
        if(strcmp(LABEL[i].name,str)==0){
            return i;}
    }
    return -1;
}
                                                                    //gets different tokens depending on table name
char* getFromTable(void* table){
    char* temp=NULL;
    if(table == sntab){                                             //if table is statement name table
        temp = ((struct table*)(table))[stenum].name;
    }
    else if(table == OPNTAB_STRING) {                               //if opntab string table
        temp = ((struct table *) (table))[stenum].val.str;
   }
    else if(table==vntab){
        int stm=-1;
        int i=0;
        int first=0;
        while(stm!=stenum){
            if(vntab[i] & 0x80 ) {
                stm++;
                if(vntab[i+1]!='\0')
                    first=i+1;
            }
            if(vntab[i] == '\0')
                return NULL;
            i++;
        }
        memcpy(tempArr,vntab+first,i-first);
        tempArr[i-first-1]&=0x7f;
        tempArr[i-first]='\0';
        return tempArr;
    }
    return temp;
}

int search(void* table ,int SRCNXT) {                               //It is a deus ex machina searches tokens in different tables
    int size;                                                       //it is size of searching token
    int error;
    int situation=0;
    char* temp;                                                     //it will be the token which will be searched
    if(!SRCNXT)                                                     //if srcnxt==1 it will not initialize stenum as -1 so the search function will continue to search from last index instead of stenum=0
        stenum=-1;
    while(1){
        switch (situation) {                                        //gets new token from table
            case 0: stenum++;
                bufferIndex=cix;
                tableIndex=0;
                if(getFromTable(table) == NULL)
                    return 1;
                error =0;                                           //error is 0 if all the char comparisons are true
            case 1: if(inbuff[bufferIndex] == '.'){                 //wild card check
                    situation=5;
                    break;
                }
            case 2:

                temp=getFromTable(table);                           //it gets token from table and comparises buffer char and token char
                size=strlen(temp);
                if(inbuff[bufferIndex] == temp[tableIndex]){        //if characters are matched
                    situation=3;                                    //goes to check next characters
                    break;
                }
                error=1;                                            //if characters arent matched makes error flag as 1
            case 3: bufferIndex++;                                  //increments indexes to check next chars
                tableIndex++;
                if(tableIndex < size){
                    situation=2;                                    //if size > table index search function goes to get new token from table
                    break;
                }                                                   //if table index >= size of buffer token it is time to make decision whether buffer token and table token are matched
                if(error==0) {                                      //if error flag was not set program returns 0; 0 return means the token was found
                    return 0;
                }
                situation=0;                                        //else goes to get and check new token from table
        }
    }
}
int opnTabSearch(char* str){                                        //searches str token in operator name table and returns index of the token if the token was found
    for(int i=0;i<55;i++){
        if(strcmp(OPNTAB[i].name, str) == 0){
            return i;
        }
    }
    return -1;
}
void setcode(char* a) {                                             //setcode function puts 1 byte to outbuff
    if(a==NULL)
        outbuff[cox++]= '\0';
    else { outbuff[cox++] = *a; }

    printOUTBUFF("SETCODE: ");
    if (cox==0) { printf("line is too long"); }
}

short getlnum() {                                                   //calculates line number and puts to outbuff
    short lNum=0;
    while(inbuff[cix]<=57 && inbuff[cix]>=48){
        lNum*=10;
        lNum+=(short)(inbuff[cix++]-'0');
    }
    printINBUFF("GETLNUM: ");
    binint=lNum;
    char low = (char)(lNum&0xff00);
    char high = (char)(lNum&0x00ff);
    setcode(&high);
    setcode(&low);
    return lNum;
}

void skblank(){                                                     //skips all blanks in inbuff until a non blank character appear
    while (inbuff[cix]==' ')
        cix++;
    printINBUFF("SKBLANK: ");
}

int getstmt(){

    char low = (char)(tslnum&0xff00);
    char high = (char)(tslnum&0x00ff);

    if(stmtab[0] == low && stmtab[1]==high){
        stmcur = &stmtab[4];
        savcur = stmcur ;
        stmcur = stmtab;
        return 0;
    }
    int i =5;
    while(stmtab[i] != '\0'){
        if(stmtab[i] == '\r'){
            if(stmtab[i+1] == low && stmtab[i+2]==high){
                stmcur = &stmtab[i+5];
                savcur = stmcur ;
                stmcur = stmtab;
                return 0;
            }
        }
    }
    savcur = stmcur ;
    stmcur = stmtab;
    return 1;
}

int getll(){
    char* temp = stmcur;
    int len = 0;
    for (; *temp != '\n' ; temp++){
        len++;
    }
}
void synin(){
        int i=cox; // TODO: ASK SERKAN
        do {
            i--; // SYN7
            stmcur[i] = outbuff[i];          //kontrol edilecek

        } while(i!=0); // TYA ; BNE :SYN7 // PREVIOUSLT i != -2
}
void syncon(){
        push(cox);                 //azaltılacak alan
        temp = stmcur+temp;
        stmcur = temp;
        contflow(stmcur,temp);
    }