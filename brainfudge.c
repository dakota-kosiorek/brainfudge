#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "brainfudge.h"

#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef __linux__
    #include <termios.h>
    #include <unistd.h>
    #include <signal.h>

    struct termios original_termios;
    // Function to set terminal attributes for non-blocking input
    void setNonBlockingMode() {
        struct termios ttystate;

        // Get the original terminal state
        tcgetattr(STDIN_FILENO, &original_termios);

        // Copy the original settings
        tcgetattr(STDIN_FILENO, &ttystate);

        // Turn off canonical mode and echoing
        ttystate.c_lflag &= ~(ICANON | ECHO);

        // Set the minimum number of characters to read
        ttystate.c_cc[VMIN] = 1;

        // Set the timeout to 0 to make it non-blocking
        ttystate.c_cc[VTIME] = 0;

        // Set the terminal attributes
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
    }

    // Function to restore the original terminal settings
    void restoreOriginalMode() {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    }

    // Signal handler for SIGINT (Ctrl+C)
    void handleSigInt(int signo) {
        restoreOriginalMode();
        exit(signo);
    }
#endif

// Brainfudge is a brainfuck interpreter written in c
// More info on brainfuck can be found at 
//      https://esolangs.org/wiki/Brainfuck#Language_overview

const int NUM_TOKEN_SYMBOLS = 8;
const char TOKEN_SYMBOLS[8] = {'>', '<', '+', '-', '.', ',', '[', ']'};

bool BF_interpret(BF_interpreterInfo *interpreterInfo) {
    #ifdef __linux__
        signal(SIGINT, handleSigInt);
    #endif

    char *sourceCode = getSourceCode(interpreterInfo->fileName);
    
    BF_LexerInfo lexer = BF_lexer(sourceCode);
    free(sourceCode);

    BF_ParseTreeNode *parseTree = BF_parser(&lexer);
    free(lexer.tokens);


    BF_CellArray cellArray = BF_initializeCells(interpreterInfo->numCells);
    BF_ParseTreeNode *instPtr = parseTree;
    BF_Cell *memPtr = cellArray.cells;
    BF_execute(parseTree, &cellArray, instPtr, memPtr);

    if (interpreterInfo->dumpMemory) {
        BF_printCells(&cellArray);
    }

    free(cellArray.cells);
    BF_freeTree(parseTree);

    #ifdef __linux__
        restoreOriginalMode();
        printf("\r\n");
    #endif

    return true;
}

BF_LexerInfo BF_lexer(char *sourceCode) {
    BF_LexerInfo li;
    li.tokenCount = 0;
    li.tokens = NULL;

    bool isCharKeyword;

    // Starting number of elements for the token array
    unsigned int tokenListSize = 32;
    unsigned int tokenCount = 0;
    BF_Token *tokens = malloc(tokenListSize * sizeof(BF_Token));
    if (tokens == NULL) {
        printf("ERROR: BF could not allocate token list!\n");
        exit(1);
    }
    
    char cd;
    int charKeyword;
    int i = 0;
    // Loop through source code and put valid tokens into `tokens` array
    while (sourceCode[i]) {
        cd = sourceCode[i];
        
        // Is the current character a language keyword
        charKeyword = isValueInArray(cd, TOKEN_SYMBOLS, NUM_TOKEN_SYMBOLS);
        if (charKeyword > -1) {
            // If the token array is not large enough, get more memory
            if (tokenCount > tokenListSize - 1) {
                tokenListSize += 32;
                tokens = realloc(tokens, tokenListSize * sizeof(BF_Token));

                if (tokens == NULL) {
                    return li;
                }
            }

            // Add current token into token array
            tokens[tokenCount] = charKeyword;

            tokenCount++;
        }

        i++;
    }

    li.tokenCount = tokenCount;
    li.tokens = tokens;

    return li;
}

BF_ParseTreeNode *BF_parser(BF_LexerInfo *li) {
    BF_ParseTreeNode *program = BF_createNode(BF_START);
    BF_ParseTreeNode *currentCommand = program;

    for (unsigned int i = 0; i < li->tokenCount; ++i) {
        switch (li->tokens[i]) {
            case BF_MV_RIGHT:
                BF_addChild(currentCommand, BF_createNode(BF_MV_RIGHT));
                break;
            case BF_MV_LEFT:
                BF_addChild(currentCommand, BF_createNode(BF_MV_LEFT));
                break;
            case BF_INC:
                BF_addChild(currentCommand, BF_createNode(BF_INC));
                break;
            case BF_DEC:
                BF_addChild(currentCommand, BF_createNode(BF_DEC));
                break;
            case BF_OUT:
                BF_addChild(currentCommand, BF_createNode(BF_OUT));
                break;
            case BF_IN:
                BF_addChild(currentCommand, BF_createNode(BF_IN));
                break;
            case BF_JMP_P:
                BF_ParseTreeNode* loop = BF_createNode(BF_JMP_P);
                BF_addChild(currentCommand, loop);
                currentCommand = loop;
                break;
            case BF_JMP_B:
                BF_addChild(currentCommand, BF_createNode(BF_JMP_B));
                currentCommand = currentCommand->parent;
                break;
            default:
                printf("Unknown Node\n");
        }
    }

    return program;
}

BF_ParseTreeNode* BF_createNode(BF_Token type) {
    BF_ParseTreeNode *newNode = malloc(sizeof(BF_ParseTreeNode));

    if (newNode == NULL) {
        printf("ERROR: BF could not allocate parse tree node!\n");
        exit(1);
    }

    newNode->type = type;
    newNode->parent = NULL;
    newNode->children = NULL;
    newNode->next = NULL;

    return newNode;
}

void BF_addChild(BF_ParseTreeNode *parent, BF_ParseTreeNode *child) {
    if (parent->children == NULL) {
        parent->children = child;
    } else {
        BF_ParseTreeNode *current = parent->children;

        while (current->next != NULL) {
            current = current->next;
        }

        current->next = child;
    }

    child->parent = parent;
}

void BF_printTree(BF_ParseTreeNode* root, int depth) {
    if (root == NULL) {
        return;
    }

    // Print the current node
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }

    switch (root->type) {
        case BF_MV_RIGHT:
            printf("Command(>)\n");
            break;
        case BF_MV_LEFT:
            printf("Command(<)\n");
            break;
        case BF_INC:
            printf("Command(+)\n");
            break;
        case BF_DEC:
            printf("Command(-)\n");
            break;
        case BF_OUT:
            printf("Command(.)\n");
            break;
        case BF_IN:
            printf("Command(,)\n");
            break;
        case BF_JMP_P:
            printf("Command([)\n");
            break;
        case BF_JMP_B:
            printf("Command(])\n");
            break;
        case BF_START:
            printf("Program Start\n");
            break;
        default:
            printf("Unknown Node\n");
    }

    // Recursively print children
    BF_printTree(root->children, depth + 1);

    // Print next sibling
    BF_printTree(root->next, depth);
}

void BF_freeTree(BF_ParseTreeNode *root) {
    if (root == NULL) {
        return;
    }

    // Recursively free children
    BF_freeTree(root->children);

    // Recursively free next sibling
    BF_freeTree(root->next);

    // Free current node
    free(root);
}

void BF_execute(BF_ParseTreeNode *root, BF_CellArray *cellArray, BF_ParseTreeNode *instPtr, BF_Cell *memPtr) {
    if (root == NULL) {
        return;
    }

    switch (root->type) {
        case BF_MV_RIGHT:
            if (memPtr + 1 > cellArray->cells + cellArray->size) {
                BF_execute(root->next, cellArray, instPtr + 1, memPtr);
            } else {
                BF_execute(root->next, cellArray, instPtr + 1, memPtr + 1);
            }
            break;
        case BF_MV_LEFT:
            if (memPtr - 1 < cellArray->cells) {
                BF_execute(root->next, cellArray, instPtr + 1, memPtr);
            } else {
                BF_execute(root->next, cellArray, instPtr + 1, memPtr - 1);
            }
            break;
        case BF_INC:
            memPtr->val++;
            BF_execute(root->next, cellArray, instPtr + 1, memPtr);
            break;
        case BF_DEC:
            memPtr->val--;
            BF_execute(root->next, cellArray, instPtr + 1, memPtr);
            break;
        case BF_OUT:
            #ifdef __linux__
                write(STDOUT_FILENO, &memPtr->val, 1);
            #endif
            #ifdef _WIN32
                putchar(memPtr->val);
            #endif
            BF_execute(root->next, cellArray, instPtr + 1, memPtr);
            break;
        case BF_IN:
            char c = BF_getChar();
            memPtr->val = c;
            BF_execute(root->next, cellArray, instPtr + 1, memPtr);
            break;
        case BF_JMP_P:
            if (memPtr->val != 0) {
                BF_execute(root->children, cellArray, instPtr + 1, memPtr);
            } else {
                BF_execute(root->next, cellArray, instPtr + 1, memPtr);
            }
            break;
        case BF_JMP_B:
            if (memPtr->val != 0) {
                BF_execute(root->parent, cellArray, root->parent, memPtr);
            } else {
                BF_execute(root->parent->next, cellArray, instPtr + 1, memPtr);
            }
            break;
        case BF_START:
            BF_execute(root->children, cellArray, instPtr + 1, memPtr);
            break;
        default:
            printf("Unknown Node\n");
    }
}

void BF_printCells(BF_CellArray *cellArray) {
    unsigned int n = cellArray->size;
    int numDigits = 0;

    do {
        n /= 10;
        ++numDigits;
    } while (n != 0);

    if (numDigits < 3) {
        numDigits = 3;
    }

    printf("\n");

    printf("Cell Num:       ");
    for (unsigned int i = 0; i < cellArray->size; ++i) {
        printf("%*d ", numDigits, i);
    }

    printf("\n");

    printf("Cell Contents:  ");
    for (unsigned int i = 0; i < cellArray->size; ++i) {
        printf("%*d ", numDigits, cellArray->cells[i].val);
    }

    printf("\n");
}

BF_CellArray BF_initializeCells(int numCells) {
    BF_CellArray cellArray;
    cellArray.size = 0;
    cellArray.cells = NULL;

    BF_Cell *cells = malloc((numCells) * sizeof(BF_Cell));

    if (cells == NULL) {
        printf("ERROR: BF could not allocate cells!\n");
        exit(1);
    }

    // Initialize all cells to have a value of 0
    for (unsigned int i = 0; i < numCells; ++i) {
        cells[i].val = 0;
    }

    cellArray.size = numCells;
    cellArray.cells = cells;

    return cellArray;
}

bool isStringNum(char *str) {
    int i = 0;
    bool valid = true;

    while(str[i]) {
        if (!isdigit(str[i])) {
            valid = false;
        }
        
        i++;
    }

    return valid;
}

char *getSourceCode(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    unsigned long int fileSize = 0;
    char *sourceCode;

    // Get the size of the file
    if (fp == NULL) {
        printf("ERROR: Cannot open file '%s'\n", fileName);
        fclose(fp);
        exit(1);
    } else if (fseek(fp, 0, SEEK_END) < 0){
        fclose(fp);
    } else {
        fileSize = ftell(fp);
        // Move file pointer back to the start of the file
        fseek(fp, 0, SEEK_SET);
    }

    // If the file has contents, allocate memory and copy file contents into the memory
    if (fileSize > 0) {
        sourceCode = malloc((fileSize + 1) * sizeof(char));

        if (sourceCode == NULL) {
            printf("ERROR: BF could not allocate cstring for source code!\n");
            exit(1);
        }

        // Read everything from the file and put it into `sourceCode`
        fread(sourceCode, fileSize, 1, fp);
        fclose(fp);
    }

    return sourceCode;
}

char *getFileNameExtension(char *fileName) {
    // Get a pointer to where `.` is in `fileName`
    char *split = strchr(fileName,'.');
    int extLen = fileName + strlen(fileName) - (split + 1);
    char *fExt = malloc((extLen + 1) * sizeof(char));

    if (fExt == NULL) {
        printf("ERROR: BF could not allocate cstring for file name!\n");
        exit(1);
    }

    // Copy over the file extension from `fileName` into fExt
    strcpy(fExt, fileName + strlen(fileName) - extLen);
    
    int i = 0;
    char ch;
    // Convert `fileName` extension to be all lowercase
    while (fExt[i]) {
        fExt[i] = tolower(fExt[i]);
        i++;
    }

    return fExt;
}

int isValueInArray(char cd, const char *arr, const int arrLen) {
    unsigned int i;

    for (i = 0; i < arrLen; ++i) {
        if (cd == arr[i]) {
            return i;
        }
    }

    return -1;
}

char BF_getChar() {
    char c = '\0';
    bool shiftDown = false;

    #ifdef __linux__
        setNonBlockingMode();
        read(STDIN_FILENO, &c, 1);

        if (c == '\r' || c == '\n') {
            c = '\0';
        }

        restoreOriginalMode();

        return c;
    #elif _WIN32
        HANDLE handleStdInput = GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD inputRecord;
        DWORD dwEventsRead;

        while (1) {
            // Wait for input event
            WaitForSingleObject(handleStdInput, INFINITE); 

            if (ReadConsoleInputA(handleStdInput, &inputRecord, 1, &dwEventsRead) && 
                    inputRecord.EventType == KEY_EVENT && 
                    inputRecord.Event.KeyEvent.bKeyDown) {
                
                if (inputRecord.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED) {
                    shiftDown = true;
                } else if (!(inputRecord.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED)) {
                    shiftDown = false;
                }

                c = inputRecord.Event.KeyEvent.uChar.AsciiChar;

                if (c >= 32 && c <= 126) {
                    if (shiftDown && c >= 'a' && c <= 'z') {
                        c = toupper(c);
                    }

                    return c;
                }

                if (c == '\r' || c == '\n') {
                    c = 0;
                    return c;
                }
            }
        }
    #endif 

    // On failure
    return '\0';
}

bool doesFileExist(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    bool exists = false;

    if (fp != NULL)
    {
        exists = true;
        fclose(fp); // close the file
    }

    return exists;
}