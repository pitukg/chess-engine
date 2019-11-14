#include "evaluation.h"


Score evaluate(Board *board) {

    // for now it is the score relative to side to move
    return ((1 - (board->meta.sideToMove << 1)) * board->score);

}




/*  CODE GENERATING BLACK TABLES FROM WHITE
    printf("\t{\n");
    for (int i = 0; i < 8; i++){
        printf("\t\t");
        for (int j = 0; j < 8; j++) {
            int square = (7-i)*8+j;
            printf("%3d,", -???PieceSquareTable[square]);
        }
        printf("\n");
    }printf("\t},\n");return 0;
*/


