#ifndef BRAINFUDGE_H
#define BRAINFUDGE_H 1

#ifndef BF_DEFAULT_CELL_COUNT
#define BF_DEFAULT_CELL_COUNT 32768
#endif

#include <stdbool.h>

extern const int NUM_TOKEN_SYMBOLS;
extern const char TOKEN_SYMBOLS[8];

enum BF_Token {
    BF_MV_RIGHT,   // '>' Move the pointer to the right
    BF_MV_LEFT,    // '<' Move the pointer to the left
    BF_INC,        // '+' Increment the memory cell at the pointer
    BF_DEC,        // '-' Decrement the memory cell at the pointer
    BF_OUT,        // '.' Output the character signified by the cell at the pointer
    BF_IN,         // ',' Input a character and store it in the cell at the pointer
    BF_JMP_P,      // '[' Jump past the matching ] if the cell at the pointer is 0
    BF_JMP_B,      // ']' Jump back to the matching [ if the cell at the pointer is nonzero
    BF_START
} typedef BF_Token;

typedef struct BF_LexerInfo {
    unsigned int tokenCount;
    BF_Token *tokens;
} BF_LexerInfo;

typedef struct BF_ParseTreeNode {
    BF_Token type;
    struct BF_ParseTreeNode* parent;
    struct BF_ParseTreeNode *children;
    struct BF_ParseTreeNode *next;
} BF_ParseTreeNode;

typedef struct BF_Cell {
    unsigned char val;
} BF_Cell;

typedef struct BF_CellArray {
    unsigned int size;
    BF_Cell *cells;
} BF_CellArray;

typedef struct BF_interpreterInfo {
    char *fileName;
    unsigned int numCells;
    bool dumpMemory;
} BF_interpreterInfo;

// Interpret brainfudge source code locate in a file supplied through the function argument and execute the brainfudge program
bool BF_interpret(BF_interpreterInfo *);

// Recognize language keywords, operators, constant and every token that the grammar defines.
BF_LexerInfo BF_lexer(char *);

// Turn tokens from lexer into a parse tree
BF_ParseTreeNode *BF_parser(BF_LexerInfo *);

void BF_printCells(BF_CellArray *);

// Run a brainfudge program based on the supplied parse tree
void BF_execute(BF_ParseTreeNode *, BF_CellArray *, BF_ParseTreeNode *, BF_Cell *);

// Set all cells to have an initial value of 0
BF_CellArray BF_initializeCells(int);

// -------- Parse Tree Functions --------

BF_ParseTreeNode *BF_createNode(BF_Token);
void BF_addChild(BF_ParseTreeNode *, BF_ParseTreeNode *);
void BF_printTree(BF_ParseTreeNode *, int);
void BF_freeTree(BF_ParseTreeNode *);

// ---------- Helper Functions ----------

// Returns true if the cstring passed in argument can be successfully converted into an int
bool isStringNum(char *);

// Returns a cstring that is `filenames` file extension
char *getFileNameExtension(char *);

// Reads in the source code from `fileName` and returns it in a cstring
char *getSourceCode(char *);

// Returns the index of the array the value is located or -1 if it does not exist in the array
int isValueInArray(char, const char *, const int);

// Get a character from the user in standard input
char BF_getChar();

bool doesFileExist(char *);

#endif