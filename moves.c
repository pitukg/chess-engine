#include "moves.h"


// gets the attack set in directions NW, N, NE, E
U64 get_positive_ray_attacks(SquareCode square, Dir dir, U64 occupied) {

    U64 attacks = rayAttacksArray[dir][square];
    U64 blocker = attacks & occupied;

    if ( blocker ) {
        square = bitscan_forward(blocker);
        attacks = attacks ^ rayAttacksArray[dir][square];
    }

    return attacks;
}

// gets the attack set in directions SE, S, SW, W
U64 get_negative_ray_attacks(SquareCode square, Dir dir, U64 occupied) {

    U64 attacks = rayAttacksArray[dir][square];
    U64 blocker = attacks & occupied;
    
    if ( blocker ) {
        square = bitscan_reverse(blocker);
        attacks = attacks ^ rayAttacksArray[dir][square];
    }

    return attacks;
}


extern inline U64 diagonal_attacks(SquareCode square, U64 occupied) {
    return get_positive_ray_attacks(square, NE, occupied)
            | get_negative_ray_attacks(square, SW, occupied);
}

extern inline U64 antidiagonal_attacks(SquareCode square, U64 occupied) {
    return get_positive_ray_attacks(square, NW, occupied)
            | get_negative_ray_attacks(square, SE, occupied);
}

extern inline U64 file_attacks(SquareCode square, U64 occupied) {
    return get_positive_ray_attacks(square, N, occupied)
            | get_negative_ray_attacks(square, S, occupied);
}

extern inline U64 rank_attacks(SquareCode square, U64 occupied) {
    return get_positive_ray_attacks(square, E, occupied)
            | get_negative_ray_attacks(square, W, occupied);
}

extern inline U64 rook_attacks(SquareCode square, U64 occupied) {
    return file_attacks(square, occupied)
            | rank_attacks(square, occupied);
}

extern inline U64 bishop_attacks(SquareCode square, U64 occupied) {
    return diagonal_attacks(square, occupied)
            | antidiagonal_attacks(square, occupied);
}

extern inline U64 queen_attacks(SquareCode square, U64 occupied) {
    return rook_attacks(square, occupied)
            | bishop_attacks(square, occupied);
}

extern inline U64 king_attacks(SquareCode square) {
    return kingAttacksArray[ square ];
}

extern inline U64 knight_attacks(SquareCode square) {
    return knightAttacksArray[ square ];
}


// pawn moves
// pawns moves are handled set-wise, in the bitboard represenation

// pawn pushes

extern inline U64 wPawn_single_push_targets(U64 wPawns, U64 occupied) {
    return ( (wPawns << 8) & ~occupied );
}

extern inline U64 bPawn_single_push_targets(U64 bPawns, U64 occupied) {
    return ( (bPawns >> 8) & ~occupied );
}


extern inline U64 wPawn_double_push_targets(U64 wPawns, U64 occupied) {
    const U64 rank4 = 0x00000000FF000000;
    U64 singlePushes = wPawn_single_push_targets(wPawns, occupied);
    return ( (singlePushes << 8) & rank4 & ~occupied );
}

extern inline U64 bPawn_double_push_targets(U64 bPawns, U64 occupied) {
    const U64 rank5 = 0x000000FF00000000;
    U64 singlePushes = bPawn_single_push_targets(bPawns, occupied);
    return ( (singlePushes >> 8) & rank5 & ~occupied );
}

// pawn attacks

extern inline U64 wPawn_east_attacks(U64 wPawns) {
    return bb_move_NE(wPawns);
}
extern inline U64 wPawn_west_attacks(U64 wPawns) {
    return bb_move_NW(wPawns);
}
extern inline U64 wPawn_any_attacks(U64 wPawns) {
    return wPawn_east_attacks(wPawns) | wPawn_west_attacks(wPawns);
}

extern inline U64 bPawn_east_attacks(U64 bPawns) {
    return bb_move_SE(bPawns);
}
extern inline U64 bPawn_west_attacks(U64 bPawns) {
    return bb_move_SW(bPawns);
}
extern inline U64 bPawn_any_attacks(U64 bPawns) {
    return bPawn_east_attacks(bPawns) | bPawn_west_attacks(bPawns);
}


// checks if a squre is attacked by any piece of the opposite color
U64 attacks_to_square(const Board *board, SquareCode square, Color attackedBy) {
    return attackedBy ?
        (
                (king_attacks(square)                   & board->pieceBB[bKing])
            |   (knight_attacks(square)                 & board->pieceBB[bKnight])
            |   (rook_attacks(square, board->occupied)  & (board->pieceBB[bRook] | board->pieceBB[bQueen]))
            |   (bishop_attacks(square, board->occupied)& (board->pieceBB[bBishop] | board->pieceBB[bQueen]))
            |   (wPawn_any_attacks(1ll << square)       & board->pieceBB[bPawn])
        )
        :
        (
                (king_attacks(square)                   & board->pieceBB[wKing])
            |   (knight_attacks(square)                 & board->pieceBB[wKnight])
            |   (rook_attacks(square, board->occupied)  & (board->pieceBB[wRook] | board->pieceBB[wQueen]))
            |   (bishop_attacks(square, board->occupied)& (board->pieceBB[wBishop] | board->pieceBB[wQueen]))
            |   (bPawn_any_attacks(1ll << square)       & board->pieceBB[wPawn])
        );
}


SquareCode smallest_attacker(const Board *board, SquareCode square, PieceType *pieceToReturn) {

    Color sideToAttack = board->meta.sideToMove;
    U64 attackers = attacks_to_square(board, square, sideToAttack);

    for (PieceType pt = (PieceType)sideToAttack; pt < 13; pt += 2) {
        U64 attackerPieces = board->pieceBB[pt] & attackers;
        if (attackerPieces) {
            *pieceToReturn = pt;
            return bitscan_forward(attackerPieces);
        }
    }

    *pieceToReturn = none;
    return 0ll;

}


// used for `lazy` move legality checking: the move wasn't legal if opponent can take the king now
extern inline U64 attacks_to_king(const Board *board, Color colorOfKing) {
    return attacks_to_square(board, board->kingSquare[colorOfKing], !colorOfKing);
}



 // retrieving the details of a `Move`

extern inline SquareCode get_from(Move move) {
    return ((move >> 6) & 0x3f);
}
extern inline SquareCode get_to(Move move) {
    return (move & 0x3f);
}

extern inline Move construct_move(SquareCode from, SquareCode to, unsigned int flags) {
    return ((flags) | ((from & 0x3f) << 6) | (to & 0x3f));
}








