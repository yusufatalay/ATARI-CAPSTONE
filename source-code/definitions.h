#ifndef ATARI_ASSEMBLER_DEFINITIONS_H
#define ATARI_ASSEMBLER_DEFINITIONS_H
struct label{
    char* name;
    short location;
};
union value{
    long num;
    char* str;
};
struct table{
    char* name;
    union value val;
};


struct table OPNTAB_STRING[71];
struct table sntab[200];
struct label LABEL[120];
struct table OPNTAB[55];

short program[20000];

char inbuff[255], cix, maxcix=0x7f,
outbuff[255],cox;

char vntab[1000]; int VNTAB_LEVEL;
char stmtab[1000]; int STMTAB_LEVEL; // statement table

int binint=0;
int stenum;
int stmlbd,stmstart;
int direct_statement;

int stack[200];
int cpc,code,stklvl;
int tvscix,tscox;

char svontx,svontc,svontl,svvvte=0;

short linelength;
char* stmcur;
char* savcur;
short tslnum;
int bufferIndex;
int tableIndex;

char tempArr[255];

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[96m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_GRAY         "\x1b[90m"
#define ANSI_COLOR_BR_RED         "\x1b[91m"
#define ANSI_COLOR_BRIGHT_GREEN        "\x1b[92m"
#define ANSI_COLOR_BR_YELLOW        "\x1b[93m"
#define ANSI_COLOR_BR_MAGENTA        "\x1b[95m"
#endif //ATARI_ASSEMBLER_DEFINITIONS_H
