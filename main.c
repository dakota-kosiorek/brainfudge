#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "brainfudge.h"

int main(int argc, char *argv[]) {
    BF_interpreterInfo interpreterInfo;
    interpreterInfo.fileName = NULL;
    interpreterInfo.numCells = BF_DEFAULT_CELL_COUNT;
    interpreterInfo.dumpMemory = false;

    if (argc < 2) {
        printf("ERROR: NO bf file!\n");
        exit(1);
    }

    char *fExt = getFileNameExtension(argv[1]);
    if ((strcmp(fExt, "bf") == 0 || strcmp(fExt, "b") == 0) && doesFileExist(argv[1])) {
        interpreterInfo.fileName = argv[1];
    } else {
        free(fExt);
        printf("ERROR: Invalid file extension or file does not exist!");
        exit(1);
    }
    free(fExt);

    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-c") == 0 && (i + 1 < argc)) {
            if (isStringNum(argv[i + 1])) {
                interpreterInfo.numCells = atoi(argv[i + 1]);
                ++i;
            } else {
                printf("ERROR: Improper number of cells!\n");
                exit(1);
            }
        }
        
        if (strcmp(argv[i], "-d") == 0) {
            interpreterInfo.dumpMemory = true;
        }
    }

    BF_interpret(&interpreterInfo);
    
    return 0;
}