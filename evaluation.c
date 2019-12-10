#include "evaluation.h"
#include "moves.h"
#include "move_generation.h"


Score evaluate(Board *board) {

    // for now it is the score relative to side to move
    return ((1 - (board->meta.sideToMove << 1)) * board->score);

}


// see "https://www.chessprogramming.org/Static_Exchange_Evaluation"
// note: evaluates for the side to move
Score static_exchange_evaluation(SquareCode square, Board *board, Stack *stack) {
    Score value = 0;
    PieceType pt = none;
    SquareCode from = smallest_attacker(board, square, &pt);

    if (pt != none) {

        // piece we are capturing is the piece on `square`
        PieceType capturedPiece = board->pieceCode[square];

        // TODO: always consider en passant captures, don't consider them here
        //     ; always consider promotions, don't consider them here
        //     ; therefore we consider captures with only the standard capture flag here
        Move capture = construct_move(from, square, captureFlag);

        make_move(board, capture, stack);

        // evaluate the exchange for the other side on the modified `board`
        value = pieceScore[capturedPiece] - static_exchange_evaluation(square, board, stack);
        if (value < 0) {
            // 'stand pat' is a better option
            value = 0;
        }

        unmake_move(board, capture, stack);

    }

    return value;

}

Score capture_static_evaluation(SquareCode from, SquareCode to, Board *board, Stack *stack) {

    Score value = 0;
    PieceType pt = board->pieceCode[from];
    PieceType capturedPiece = board->pieceCode[to];

    // TODO: always consider en passant captures, don't consider them here
    //     ; always consider promotions, don't consider them here
    //     ; therefore we consider captures with only the standard capture flag here
    Move capture = construct_move(from, to, captureFlag);

    make_move(board, capture, stack);

    value = pieceScore[capturedPiece] - static_exchange_evaluation(to, board, stack);

    unmake_move(board, capture, stack);

    return value;

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


