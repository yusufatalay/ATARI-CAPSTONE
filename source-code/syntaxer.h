#include <ctype.h>
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

