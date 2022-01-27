#ifndef HC_BOARD_H
#define HC_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "piece.h"

typedef struct Board
{
	DA_hc_pieces *da_pieces;
	unsigned en_passant_possible;
} HC_Board;

HC_Board * HC_Board_create();
void HC_Board_destroy(HC_Board *board);
void HC_Board_clear(HC_Board *board);
void HC_Board_copy(HC_Board *dest, HC_Board *src);

int HC_Board_add_piece(HC_Board *board, HC_PieceType type, HC_Color color, HC_Coordinates coordinates,
					   unsigned moved);
int HC_Board_remove_piece(HC_Board *board, HC_Coordinates coordinates);
int HC_Board_square_is_free(HC_Board *board, HC_Coordinates coords);
HC_Piece *HC_Board_get_square(HC_Board *board, HC_Coordinates coords);

long int HC_Board_get_piece_index(HC_Board *board, HC_Piece *piece);

void HC_Board_default_position(HC_Board *board);

#ifdef __cplusplus
}
#endif

#endif
