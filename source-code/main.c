#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basicFunctions.h"
#include "definitions.h"
#include "syntaxer.h"
#include "initializer.h"
#include "constantFunctions.h"
#include "variableFunctions.h"

int main(void) { // A.K.A syntax (label name) or editor
    initializeTables();                                  //reads tables statement name table, operator token table, syntax table and
    //creates program[] array what is syntax table
    while (1) {                                          //program editor loop: takes input from and tokenizes
	syntax();
    }

}

/*
 * STATEMENT TABLE FORMAT
 *    [][]          []            []                      []            [...]   []         []                      []            [...]   []    []
 * linenumber | linelength | statement length | statement name token | tokens | eos | statement length | statement name token | tokens | eos | eol
 */
