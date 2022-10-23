int tvar(int tvtype){
    skblank();
    tvscix=cix;
    if(isalpha(inbuff[cix])) {
        srcont();                               //which table?
        if (svontc == 0 || (svontc == 1) && (inbuff[svontl] >= 0x30)) {  //not reserved word, or it is
            do                                                  //a non-reserved word whose prefix
            {   cix++;
                printINBUFF("TVAR: ");
            }while (isalpha(inbuff[cix]) || isdigit(inbuff[cix]));



        if (tvtype == 0x80 ) {
            if (inbuff[cix] == '$') {
                cix++;
                printINBUFF("TVAR STR: ");
            }                       //skip over $
            else
                return 1;
        }
        else if ( tvtype == 0x00 ) {
            if (inbuff[cix] == '(') {
                cix++;      //skip over (
                printINBUFF("TVAR NUMERIC ARRAY: ");
                tvtype += 0x40;
            }
        }
        char temp=cix;
        cix=tvscix;    //search expects the string to be searched pointed by cix.
        tvscix=temp;  //variable string'in sonunu tvscix'e attık.. search
        int result=search(vntab,0);
        while(!result){
            if(bufferIndex==tvscix){
                break;
            }
            else{
                result=search(vntab,1);
            }
        }
        if (result == 1){
            int len=tvscix-cix;
            int i=0;
            do {
                vntab[VNTAB_LEVEL++] = inbuff[cix+i];
                i++;
                len--;
            } while (len!=0);
            vntab[VNTAB_LEVEL-1] |= 0x80;

            len =8;
            i=0;
            do {
                stmtab[STMTAB_LEVEL++] = '\0';
                i++;
                len--;
            } while (len!=0);
            svvvte++;
        }
            if(tvtype==0x40)
                --tvscix;
            cix=tvscix;
            if (stenum>0x7f)
                return 1;
		char index =(stenum+128);
            setcode(&index);
            return 0;

        }
        return 1;
    }
    else
        return 1;
}

int tnvar(){
    return tvar(0x00);
}

int tsvar(){
    return tvar(0x80);
}
