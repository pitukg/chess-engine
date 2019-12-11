// file for debugging utilities, unit tests
// and performance test (PERFT)
#include <stdio.h>
#include "board.h"
#include "moves.h"
#include "move_generation.h"
#include "evaluation.h"
#include "search.h"

const char pieceToStringArr[13][8] = {
    "wPawn",
    "bPawn",
    "wKnight",
    "bKnight",
    "wBishop",
    "bBishop",
    "wRook",
    "bRook",
    "wQueen",
    "bQueen",
    "wKing",
    "bKing",
    "none"
};
// to string function for debugging
const char *piece_to_string(PieceType pt) {
    return (&pieceToStringArr[pt][0]);
}

const char squareToStringArr[64][3] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};
// to string function for debugging
const char *square_to_string(SquareCode square) {
    return (&squareToStringArr[square][0]);
}



// print utility for debugging purposes
void print_bitboard(U64 board) {
//    printf("%#llx, ", board); return;
    printf("%#llx\n", board);
    for (int i = 7; i >= 0; i--) {
        printf(" %d", i+1);
        for (int j = 0; j < 8; j++)
            printf(" %c", (bb_get_bit(&board, i*8+j) ? 'X' : '.'));
        printf("\n");
    }
    printf("   a b c d e f g h\n");
}

void print_move(Move move, PieceType pt) {
    printf("{%s.%s%s, %d, %d}\n", piece_to_string(pt), square_to_string(get_from(move)), square_to_string(get_to(move)), ((move & captureFlag)!=0), ((move & promotionFlag)!=0));
}


// naive implementation for debugging, don't use it!
#include <string.h>
char *to_FEN(Board *board) {

    char *FEN;
    char FENarr[100] = {""};
    FEN = FENarr;

    for (int firstSquareInRank = a8; firstSquareInRank >= 0; firstSquareInRank -= 8) {
        int emptyCounter = 0;
        for (SquareCode square = firstSquareInRank; square != firstSquareInRank+8; ++square) {
            PieceType pt = board -> pieceCode[square];
            if (pt == none) {
                emptyCounter++;
            } else {
                if (emptyCounter) {
                    sprintf(FEN + strlen(FEN), "%c", ('0' + emptyCounter));
                    emptyCounter = 0;
                }
                char pc;
                switch(pt) {
                    case wPawn: pc = 'P'; break;
                    case bPawn: pc = 'p'; break;
                    case wKnight: pc = 'N'; break;
                    case bKnight: pc = 'n'; break;
                    case wBishop: pc = 'B'; break;
                    case bBishop: pc = 'b'; break;
                    case wRook: pc = 'R'; break;
                    case bRook: pc = 'r'; break;
                    case wQueen: pc = 'Q'; break;
                    case bQueen: pc = 'q'; break;
                    case wKing: pc = 'K'; break;
                    case bKing: pc = 'k'; break;
                    default: pc = 'X';
                }
                sprintf(FEN + strlen(FEN), "%c", pc);
            }
        }
        if (emptyCounter) {
            sprintf(FEN + strlen(FEN), "%c", ('0' + emptyCounter));
        }
        if (firstSquareInRank) sprintf(FEN + strlen(FEN), "/");
    }
    sprintf(FEN + strlen(FEN), " %c ", (board -> meta.sideToMove ? 'b' : 'w'));

    if (!(board -> meta.kingsideCastlingRight[WHITE] | board -> meta.kingsideCastlingRight[BLACK]
            | board -> meta.queensideCastlingRight[WHITE] | board -> meta.queensideCastlingRight[BLACK])) {
        sprintf(FEN + strlen(FEN), "-");
    } else {
        if (board -> meta.kingsideCastlingRight[WHITE]) sprintf(FEN + strlen(FEN), "K");
        if (board -> meta.queensideCastlingRight[WHITE]) sprintf(FEN + strlen(FEN), "Q");
        if (board -> meta.kingsideCastlingRight[BLACK]) sprintf(FEN + strlen(FEN), "k");
        if (board -> meta.queensideCastlingRight[BLACK]) sprintf(FEN + strlen(FEN), "q");
    }

    if (!board -> meta.enPassantTargets) {
        sprintf(FEN + strlen(FEN), " -");
    } else {
        SquareCode eptarget = bitscan_forward(board -> meta.enPassantTargets);
        sprintf(FEN + strlen(FEN), " %s", square_to_string(eptarget));
    }

    sprintf(FEN + strlen(FEN), " %d %d", board -> meta.halfmoveClock, board -> meta.fullmoveCounter);
    char *FENptr = FEN;
    return FENptr;
}





// PERFT

// utility global variables for adding split to perft:
//int baseDepth, splitDepth;

// dfs for perft
int dfs(Board *board, Stack *stack, int depth) {

    if (!depth) return 1;

    int ret = 0;
    Move begin[10000], *end;

    end = generate_pseudolegal_moves(begin, board->meta.sideToMove, board);


    for (Move *moveIterator = begin; moveIterator != end; ++moveIterator) {

        make_move(board, *moveIterator, stack);

        if ((attacks_to_king(board, !(board->meta.sideToMove)) == 0ll)) {

//            print_move( *moveIterator, board->pieceCode[ get_to(*moveIterator) ] );
//            printf("score: %d.%d\n", board->score / 100, board->score % 100);

            ret += dfs(board, stack, depth-1);

        }
        
        unmake_move(board, *moveIterator, stack);

    }
    return ret;
}


int perft(char *FEN, int depth) {
    Board board = parse_FEN(FEN);
    Stack stack;
//    baseDepth = depth; splitDepth = 1;
    return dfs(&board, &stack, depth);
}


// checker against perft results

#include <time.h>
void perft_check() {

    char *FEN[7] = {"",
                    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
                    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
                    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
                    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
                    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"
                };
    int perft_results[7][5] = {{0, 0, 0, 0, 0},
                               {1, 20, 400, 8902, 197281},
                               {1, 48, 2039, 97862, 4085603},
                               {1, 14, 191, 2812, 43238},
                               {1, 6, 264, 9467, 422333},
                               {1, 44, 1486, 62379, 2103487},
                               {1, 46, 2079, 89890, 3894594}
                           };

    clock_t startTime, endTime;

    for (int test_case = 1; test_case <= 6; test_case++) {
        printf("Position %d\n", test_case);
        for (int depth = 1; depth <= 4; depth++) {
            startTime = clock();
            int testers_result = perft(FEN[test_case], depth);
            endTime = clock(); double timeTaken = (double)(endTime-startTime) / CLOCKS_PER_SEC;
            printf("%d\t...\t", testers_result);
            if (testers_result == perft_results[test_case][depth]) printf("OKAY\n");
            else printf("WRONG RESULT (should be %d)\n", perft_results[test_case][depth]);
            printf("\ttime: %fms\t nps: %f\n", timeTaken, (double)testers_result / timeTaken);
        }
        printf("\n");
    }

}



void eval_testing() {
    char FENs[5][100] = {
        "rnbqkb1r/1p3ppp/p2ppn2/8/3NP3/2N1BP2/PPP3PP/R2QKB1R b KQkq - 0 7",
        "rn1qkb1r/1p3ppp/p2pbn2/4p3/4P3/1NN1BP2/PPP3PP/R2QKB1R b KQkq - 0 8",
        "rnbqkb1r/1p3ppp/p2ppn2/8/3NP3/2N5/PPP1BPPP/R1BQK2R w KQkq - 0 7",
        "r1bqk2r/1p2bpp1/p1nppn1p/8/3NP3/2N1B3/PPPQ1PPP/2KR1B1R w kq - 0 10",
        "r1bqkb1r/5ppp/p1np1n2/1p2p1B1/4P3/N1N5/PPP2PPP/R2QKB1R w KQkq b6 0 9",
    };
    for (int pos = 0; pos < 5; pos++) {
        Board board = parse_FEN(FENs[pos]);
        Stack stack;

        Move bestMove;
        printf("position score: %.2f (relative to side to move)\n",
                (double)alphaBeta(-30000, +30000, 5, &board, &stack, &bestMove) / 100.);

    }
}



// UNIT TESTS


void promotion_test() {

    Board board = parse_FEN("2b5/PP6/8/8/8/8/4r3/5k1K w - - 0 1");
    Stack stack;
    Move moveList[256];
    Move *lastMove = generate_pawn_moves(moveList, board.meta.sideToMove, &board);
    int size = lastMove - moveList;
    for (Move *move = --lastMove; move >= moveList; --move) {
        print_move(*move, board.pieceCode[get_from(*move)]);
        make_move(&board, *move, &stack);
//        print_bitboard(board.pieceBB[bBishop]);
        unmake_move(&board, *move, &stack);
    }


}

void test_attacks(char *FEN) {

    Board board = parse_FEN(FEN);

    print_bitboard(queen_attacks(f3, board.occupied));

    return;

    printf("occupation:\n"); print_bitboard(board.occupied);
//    printf("wPawn any attacks:\n"); print_bitboard(wPawn_any_attacks(board.pieceBB[wPawn]));
//    printf("bPawn any attacks:\n"); print_bitboard(bPawn_any_attacks(board.pieceBB[bPawn]));
    printf("wRook attacks:\n"); print_bitboard(rook_attacks(a8, board.occupied));

    U64 attacks = rook_attacks(a8, board.occupied);
    do {
        printf("%d\n", bitscan_forward(attacks));
        print_bitboard(attacks);
    } while (attacks &= (attacks-1));

}

void test_movegen(char *FEN) {
    Board board = parse_FEN(FEN);
    print_bitboard(board.occupied);
    Move moveList[256];
    Move *lastMove = generate_pseudolegal_moves(moveList, board.meta.sideToMove, &board);
//    Move *lastMove = append_nonpawn_moves_from_attack_set(moveList, a8, rook_attacks(a8, board.occupied), &(board.occupied), &(board.piecesByColor[WHITE]));
    int size = lastMove - moveList;
    for (Move *move = --lastMove; move >= moveList; --move) {
        print_move(*move, board.pieceCode[get_from(*move)]);
    }
    printf("# of moves: %d\n", size);
}

void test_FEN(char *FEN) {
    Board board = parse_FEN(FEN);
    print_bitboard(board.occupied);
    Move moveList[256];
    Move *lastMove = generate_pseudolegal_moves(moveList, board.meta.sideToMove, &board);
    int size = lastMove - moveList;
    for (Move *move = --lastMove; move >= moveList; --move) {
        print_move(*move, board.pieceCode[get_from(*move)]);
    }
    printf("# of moves: %d\n", size);

}

void test_make(char *FEN) {

    Board board = parse_FEN(FEN);
    Stack stack;
    Move moveList[256];
    Move *lastMove = generate_pseudolegal_moves(moveList, board.meta.sideToMove, &board);
    int size = lastMove - moveList;
    for (Move *move = --lastMove; move >= moveList; --move) {
        print_move(*move, board.pieceCode[get_from(*move)]);
    }
    printf("# of moves: %d\n", size);
}


void stack_test() {
    Stack stack; printf("k s: %d\n", stack.size);
    StackObject sos[5], popped;sos[0].meta.halfmoveClock=10,sos[1].meta.halfmoveClock=11,sos[2].meta.halfmoveClock=12,sos[3].meta.halfmoveClock=13;
    push(sos[0], &stack);popped=pop(&stack);printf("%d\n",popped.meta.halfmoveClock);push(sos[0], &stack);
    push(sos[1], &stack);popped=pop(&stack);printf("%d\n",popped.meta.halfmoveClock);push(sos[1], &stack);
    push(sos[2], &stack);popped=pop(&stack);printf("%d\n",popped.meta.halfmoveClock);push(sos[2], &stack);
    push(sos[3], &stack);popped=pop(&stack);printf("%d\n",popped.meta.halfmoveClock);push(sos[3], &stack);
}

void check_test(char *FEN) {
    Board board = parse_FEN(FEN);
    printf("The white king is %sin check.\n", ((attacks_to_king(&board, WHITE) != 0ll) ? "" : "not "));
    printf("The black king is %sin check.\n", ((attacks_to_king(&board, BLACK) != 0ll) ? "" : "not "));
}


void smallest_attacker_test() {

    Board board = parse_FEN("3K4/3R4/8/4B3/r2p2Q1/8/3q4/2kr4 w - - 0 1");
    PieceType smallestWhite, smallestBlack;
    (void)smallest_attacker(&board, d4, &smallestWhite);
    board = parse_FEN("3K4/3R4/8/4B3/r2p2Q1/8/3q4/2kr4 b - - 0 1");
    (void)smallest_attacker(&board, d4, &smallestBlack);
    printf("w: %s\nb: %s\n", piece_to_string(smallestWhite), piece_to_string(smallestBlack));
}



/*
    CODE USED FOR GENERATING RAY ATTACKS


for (enum SquareCode _sq = a1; _sq <= h8; ++_sq) {

    U64 board = 0ll;

    //sqc = i*8 + j;
    enum Dir dir = NW; // +9
    int i_change = 1,
        j_change = -1;

    for (int i = i_change + (int)_sq / 8, j = j_change + (int)_sq % 8; 0 <= i && 0 <= j && i < 8 && j < 8; i += i_change, j += j_change) {
        enum SquareCode square = (8*i + j);
        bb_set_true(&board, square);
    }
    print_bitboard(board);

}

*/

/*
    CODE USED FOR GENERATING KING ATTACKS
void calculate_king_attacks(enum SquareCode square) {
    U64 board = 0ll, attacks = 0ll;
    bb_set_true(&board, square);
    for (enum Dir d = N; d <= NW; ++d) {
        attacks |= bb_move_one(board, d);
    }
    print_bitboard(attacks);
}
*/

/*
    KNIGHT ATTACS

void calculate_knight_attacks(U64 knights) {
    U64 NNE = bb_move_one( bb_move_one(knights, N), NE );
    U64 NEE = bb_move_one( bb_move_one(knights, E), NE );
    U64 SEE = bb_move_one( bb_move_one(knights, E), SE );
    U64 SSE = bb_move_one( bb_move_one(knights, S), SE );
    U64 SSW = bb_move_one( bb_move_one(knights, S), SW );
    U64 SWW = bb_move_one( bb_move_one(knights, W), SW );
    U64 NWW = bb_move_one( bb_move_one(knights, W), NW );
    U64 NNW = bb_move_one( bb_move_one(knights, N), NW );
    print_bitboard(NNE | NEE | SEE | SSE | SSW | SWW | NWW | NNW);
}

*/








