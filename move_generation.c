#include "move_generation.h"

// utility for nonpawn move generation
static Move *append_nonpawn_moves_from_attack_set(Move *moveList, SquareCode from, U64 attacks, const U64 *occupied, const U64 *ours);

// functions

Move *generate_castling_moves(Move *moveList, Color color, const Board *board) {

    // kingside
    if  ((board->meta.kingsideCastlingRight[color])
        // check if the squares are not occupied by other pieces
     && ((emptySquaresForKingsideCastling[color] & board->occupied) == 0ll)
        // check if there is still a rook in the corner i. e. if it has not been captured yet
     && (board->pieceCode[ (color ? h8 : h1) ] == (color ? bRook : wRook))
        // check if the squares are not attacked by opponent's pieces    
     && !(color ?
    /*black*/ (attacks_to_square(board, e8, WHITE) | attacks_to_square(board, f8, WHITE) | attacks_to_square(board, g8, WHITE))
        :
    /*white*/ (attacks_to_square(board, e1, BLACK) | attacks_to_square(board, f1, BLACK) | attacks_to_square(board, g1, BLACK))
        )) {
        SquareCode from = (color == WHITE) ? e1 : e8;
        SquareCode to = (color == WHITE) ? g1 : g8;
        *moveList++ = construct_move(from, to, kingsideCastleFlag);
    }

    // queenside
    if  ((board->meta.queensideCastlingRight[color])
        // check if the squares are not occupied by other pieces
     && ((emptySquaresForQueensideCastling[color] & board->occupied) == 0ll)
        // check if there is still a rook in the corner i. e. if it has not been captured yet
     && (board->pieceCode[ (color ? a8 : a1) ] == (color ? bRook : wRook))
        // check if the squares are not attacked by opponent's pieces
     && !(color ?
    /*black*/ (attacks_to_square(board, e8, WHITE) | attacks_to_square(board, d8, WHITE) | attacks_to_square(board, c8, WHITE))
        :
    /*white*/ (attacks_to_square(board, e1, BLACK) | attacks_to_square(board, d1, BLACK) | attacks_to_square(board, c1, BLACK))
        )) {
        SquareCode from = (color == WHITE) ? e1 : e8;
        SquareCode to = (color == WHITE) ? c1 : c8;
        *moveList++ = construct_move(from, to, queensideCastleFlag);
    }

    return moveList;

}


// function for serializing the attack bitboard and appending the generated moves to the move list
// ours is the occupation of our own pieces
static Move *append_nonpawn_moves_from_attack_set(Move *moveList, SquareCode from, U64 attacks, const U64 *occupied, const U64 *ours) {
    if (attacks) do {
        SquareCode to = bitscan_forward(attacks);
        // if to is a piece of our own color move is not legal
        if ((1ll << to) & *ours)
            continue;
        unsigned int flags = ((1ll << to) & *occupied) ? captureFlag : 0;
        *moveList++ = construct_move(from, to, flags);
    } while (attacks &= (attacks-1)); // resets LSB
    return moveList;
}



Move *generate_nonpawn_moves(Move *moveList, Color color, const Board *board) {

    // bishops

    U64 pieceBB = board->pieceBB[ (color ? bBishop : wBishop) ];
    if (pieceBB) do {
        int from = bitscan_forward(pieceBB);
        moveList = append_nonpawn_moves_from_attack_set(moveList, from,
                bishop_attacks(from, board->occupied), &(board->occupied), &(board->piecesByColor[color]));

    } while (pieceBB &= (pieceBB-1)); // resets LSB

   // rooks

    pieceBB = board->pieceBB[ (color ? bRook : wRook) ];
    if (pieceBB) do {
        SquareCode from = bitscan_forward(pieceBB);
        moveList = append_nonpawn_moves_from_attack_set(moveList, from,
                rook_attacks(from, board->occupied), &(board->occupied), &(board->piecesByColor[color]));

    } while (pieceBB &= (pieceBB-1));

    // queens

    pieceBB = board->pieceBB[ (color ? bQueen : wQueen) ];
    if (pieceBB) do {
        SquareCode from = bitscan_forward(pieceBB);
        moveList = append_nonpawn_moves_from_attack_set(moveList, from,
                queen_attacks(from, board->occupied), &(board->occupied), &(board->piecesByColor[color]));

    } while (pieceBB &= (pieceBB-1));

    // knights

    pieceBB = board->pieceBB[ (color ? bKnight : wKnight) ];
    if (pieceBB) do {
        SquareCode from = bitscan_forward(pieceBB);
        moveList = append_nonpawn_moves_from_attack_set(moveList, from,
                knight_attacks(from), &(board->occupied), &(board->piecesByColor[color]));

    } while (pieceBB &= (pieceBB-1));

    // king
    // exactly one king, if is temporary for debugging
    {
        SquareCode from = board->kingSquare[color];
        moveList = append_nonpawn_moves_from_attack_set(moveList, from,
                king_attacks(from), &(board->occupied), &(board->piecesByColor[color]));
    }


    return moveList;

}


Move *generate_pawn_moves(Move *moveList, Color color, const Board *board) {

    // pawn pushes

    U64 singlePushTargets = color ? bPawn_single_push_targets(board->pieceBB[bPawn], board->occupied)
            : wPawn_single_push_targets(board->pieceBB[wPawn], board->occupied);

    if (singlePushTargets) do {

        SquareCode to = bitscan_forward(singlePushTargets);
        SquareCode from = color ? (to + 8) : (to - 8);

        // append `quiet` promotions if on the last rank
        if ((color && (to < a2)) | (!color && (to > h7))) {
            *moveList++ = construct_move(from, to, knightPromotionFlag);
            *moveList++ = construct_move(from, to, bishopPromotionFlag);
            *moveList++ = construct_move(from, to, rookPromotionFlag);
            *moveList++ = construct_move(from, to, queenPromotionFlag);
        }
        else {
            *moveList++ = construct_move(from, to, 0);
        }

    } while (singlePushTargets &= (singlePushTargets-1));

    U64 doublePushTargets = color ? bPawn_double_push_targets(board->pieceBB[bPawn], board->occupied)
            : wPawn_double_push_targets(board->pieceBB[wPawn], board->occupied);

    if (doublePushTargets) do {

        SquareCode to = bitscan_forward(doublePushTargets);
        SquareCode from = color ? (to + 16) : (to - 16);
        *moveList++ = construct_move(from, to, doublePushFlag);

    } while (doublePushTargets &= (doublePushTargets-1));


    // pawn attacks
    // normal

    // later could refactor into a single function w/ nonpawn bitboard serialization

    U64 eastAttacks = color ? bPawn_east_attacks(board->pieceBB[bPawn])
            : wPawn_east_attacks(board->pieceBB[wPawn]);
    eastAttacks &= board->piecesByColor[!color]; // occupied squares by opposite color

    if (eastAttacks) do {

        SquareCode to = bitscan_forward(eastAttacks);
        SquareCode from = color ? (to + 7) : (to - 9);
        // check for capture promotions
        if ((color && (to < a2)) || (!color && (to > h7))) {
            *moveList++ = construct_move(from, to, knightPromotionFlag | captureFlag);
            *moveList++ = construct_move(from, to, bishopPromotionFlag | captureFlag);
            *moveList++ = construct_move(from, to, rookPromotionFlag | captureFlag);
            *moveList++ = construct_move(from, to, queenPromotionFlag | captureFlag);
        } else *moveList++ = construct_move(from, to, captureFlag);

    } while (eastAttacks &= (eastAttacks-1));

    U64 westAttacks = color ? bPawn_west_attacks(board->pieceBB[bPawn])
            : wPawn_west_attacks(board->pieceBB[wPawn]);
    westAttacks &= board->piecesByColor[!color];

    if (westAttacks) do {

        SquareCode to = bitscan_forward(westAttacks);
        SquareCode from = color ? (to + 9) : (to - 7);
        // check for capture promotions
        if ((color && (to < a2)) || (!color && (to > h7))) {
            *moveList++ = construct_move(from, to, knightPromotionFlag | captureFlag);
            *moveList++ = construct_move(from, to, bishopPromotionFlag | captureFlag);
            *moveList++ = construct_move(from, to, rookPromotionFlag | captureFlag);
            *moveList++ = construct_move(from, to, queenPromotionFlag | captureFlag);
        } else *moveList++ = construct_move(from, to, captureFlag);

    } while (westAttacks &= (westAttacks-1));


    // en passant captures

    // en passant capture can't be a promotion

    U64 eastEnPassantAttacks = color ? bPawn_east_attacks(board->pieceBB[bPawn])
            : wPawn_east_attacks(board->pieceBB[wPawn]);
    eastEnPassantAttacks &= board->meta.enPassantTargets;// & possibleEnPassantTargets[color];

    if (eastEnPassantAttacks) do {

        SquareCode to = bitscan_forward(eastEnPassantAttacks);
        SquareCode from = color ? (to + 7) : (to - 9);
        *moveList++ = construct_move(from, to, enPassantCaptureFlag);
    
    } while (eastEnPassantAttacks &= (eastEnPassantAttacks-1));

    U64 westEnPassantAttacks = color ? bPawn_west_attacks(board->pieceBB[bPawn])
            : wPawn_west_attacks(board->pieceBB[wPawn]);
    westEnPassantAttacks &= board->meta.enPassantTargets;// & possibleEnPassantTargets[color];

    if (westEnPassantAttacks) do {

        SquareCode to = bitscan_forward(westEnPassantAttacks);
        SquareCode from = color ? (to + 9) : (to - 7);
        *moveList++ = construct_move(from, to, enPassantCaptureFlag);

    } while (westEnPassantAttacks &= (westEnPassantAttacks-1));


    return moveList;

}



Move *generate_pseudolegal_moves(Move *moveList, Color color, const Board* board) {

    moveList = generate_castling_moves(moveList, color, board);
    moveList = generate_nonpawn_moves(moveList, color, board);
    moveList = generate_pawn_moves(moveList, color, board);

    return moveList;

}




// make move

void make_move(Board *board, Move move, Stack *stack) {

    // create the object for pushing the present metadata into the stack
    StackObject stObj;
    stObj.meta = board -> meta;
    stObj.capturedPiece = none; // no capture in the general case

    Color color = board -> meta.sideToMove;

    SquareCode from = get_from(move), to = get_to(move);
    PieceType pt = board -> pieceCode[from];


    // turn off castling ability after a rook or king move
    if (pt == (color ? bKing : wKing))
        board -> meta.kingsideCastlingRight[color] = board -> meta.queensideCastlingRight[color] = 0;
    else if (from == (color ? a8 : a1))
        board -> meta.queensideCastlingRight[color] = 0;
    else if (from == (color ? h8 : h1))
        board -> meta.kingsideCastlingRight[color] = 0;

    // setting the en passant target square after double push
    board -> meta.enPassantTargets = ((move & moveKindFlag) == doublePushFlag) ?
            (1ll << ((get_from(move) + get_to(move)) / 2)) : 0ll;

    // handling castles
    if ((move & moveKindFlag) == kingsideCastleFlag) {

        remove_piece(board,
            (color ? bKing : wKing),
            (color ? e8 : e1)
        );
        add_piece(board,
            (color ? bKing : wKing),
            (color ? g8 : g1)
        );
        remove_piece(board,
            (color ? bRook : wRook),
            (color ? h8 : h1)
        );
        add_piece(board,
            (color ? bRook : wRook),
            (color ? f8 : f1)
        );

    } else if ((move & moveKindFlag) == queensideCastleFlag) {

        remove_piece(board,
            (color ? bKing : wKing),
            (color ? e8 : e1)
        );
        add_piece(board,
            (color ? bKing : wKing),
            (color ? c8 : c1)
        );
        remove_piece(board,
            (color ? bRook : wRook),
            (color ? a8 : a1)
        );
        add_piece(board,
            (color ? bRook : wRook),
            (color ? d8 : d1)
        );

    }
    else {

        remove_piece(board, pt, from);

        if (move & captureFlag) {

            // handling en passant moves as well
            int isEnPassantCapture = ((move & moveKindFlag) == enPassantCaptureFlag);
            stObj.capturedPiece = isEnPassantCapture ? (color ? wPawn : bPawn) : board -> pieceCode[to];

            remove_piece(board, stObj.capturedPiece, (isEnPassantCapture ? (color ? (to+8) : (to-8)) : to));

        }

        // handling promotions; if a move is promotion, it can't be an en passant capture or double push
        pt = (move & promotionFlag) ? 
            (
                ((move & promotionKindFlag) == knightPromotionFlag) ? (color ? bKnight : wKnight) :
                (
                    ((move & promotionKindFlag) == bishopPromotionFlag) ? (color ? bBishop : wBishop) :
                    (
                        ((move & promotionKindFlag) == rookPromotionFlag) ? (color ? bRook : wRook) :
                            (color ? bQueen : wQueen)
                    )
                )
            ) : pt;

        add_piece(board, pt, to);
    
    }


    // push current state into the stack
    push(stObj, stack);

    // update board state
    board->meta.sideToMove = !color;
    board->meta.fullmoveCounter++;
    if ((move & captureFlag) || (pt == (color ? bPawn : wPawn))) { // TODO: this might be buggy, check logic
        board->meta.halfmoveClock++;
    }

}


// unmake move

void unmake_move(Board *board, Move move, Stack *stack) { // color is whose move is being unmade

    // pop the board metadata to be restored from stack
    StackObject stObj = pop(stack);

    Color color = stObj.meta.sideToMove;

    SquareCode from = get_from(move), to = get_to(move);
    PieceType pt = board -> pieceCode[to];


    if ((move & moveKindFlag) == kingsideCastleFlag) {

        remove_piece(board,
            (color ? bKing : wKing),
            (color ? g8 : g1)
        );
        add_piece(board,
            (color ? bKing : wKing),
            (color ? e8 : e1)
        );
        remove_piece(board,
            (color ? bRook : wRook),
            (color ? f8 : f1)
        );
        add_piece(board,
            (color ? bRook : wRook),
            (color ? h8 : h1)
        );

    } else if ((move & moveKindFlag) == queensideCastleFlag) {

        remove_piece(board,
            (color ? bKing : wKing),
            (color ? c8 : c1)
        );
        add_piece(board,
            (color ? bKing : wKing),
            (color ? e8 : e1)
        );
        remove_piece(board,
            (color ? bRook : wRook),
            (color ? d8 : d1)
        );
        add_piece(board,
            (color ? bRook : wRook),
            (color ? a8 : a1)
        );

    }
    else {

        remove_piece(board, pt, to);

        if (move & captureFlag) {
            // handling en passant moves as well
            add_piece(board, stObj.capturedPiece,
                ((move & moveKindFlag) == enPassantCaptureFlag) ? (color ? (to+8) : (to-8)) : to);
        }

        pt = (move & promotionFlag) ? (color ? bPawn : wPawn) : pt;

        add_piece(board, pt, from);

    }
    

    // restoring board state from stack
    board -> meta = stObj.meta;

}














