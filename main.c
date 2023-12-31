#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "brainfudge.h"

// TODO
//  malloc error handling
//  file cant open error handling
//  character input for linux systems

int main(int argc, char *argv[]) {
    BF_interpreterInfo interpreterInfo;
    interpreterInfo.fileName = NULL;
    interpreterInfo.numCells = BF_DEFAULT_CELL_COUNT;
    interpreterInfo.dumpMemory = false;

    char *fExt = getFileNameExtension(argv[1]);
    if ((strcmp(fExt, "bf") == 0 || strcmp(fExt, "b") == 0)) {
        interpreterInfo.fileName = argv[1];
    }

    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-c") == 0 && (i + 1 < argc)) {
            if (isStringNum(argv[i + 1]) == true) {
                interpreterInfo.numCells = atoi(argv[i + 1]);
                printf("Cell count: %s\n", argv[i + 1]);
            }
        }
    }

    if (argc < 3) {
        printf("ERROR: Too few arguments!\n");
    } else if (argc == 3) {
        char *fExt = getFileNameExtension(argv[1]);

        if ((strcmp(fExt, "bf") == 0 || strcmp(fExt, "b") == 0) && isStringNum(argv[2]) == true) {
            //BF_interpret(argv[1], atoi(argv[2]));
        } else {
            printf("ERROR: Improper file type or cell number value!\n");
        }

        free(fExt);
    } else {
        printf("ERROR: Too many arguments!\n");
    }
    
    return 0;
}