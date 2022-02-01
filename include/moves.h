#ifndef HC_MOVES_H_INCLUDED
#define HC_MOVES_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "piece.h"
#include "common.h"
#include "dynamic_array_gen.h"

typedef struct Move
{
	HC_Coordinates coords_from, coords_to;
	unsigned takes, check, castle, en_passant;
	int promotion;
} HC_Move;

DYNAMIC_ARRAY_GEN_DECL(HC_Move, hc_moves)

int HC_Piece_find_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *list);
int HC_player_find_moves(HC_Board *board, HC_Color color, DA_hc_moves *list);
void HC_Piece_append_moves_unsafe(HC_Board *board, HC_Piece *piece, DA_hc_moves *list);

int HC_Board_play_move(HC_Board *board, HC_Move *move);
	
char *HC_Move_get_verbose_str(HC_Board *board, HC_Move *move);
int HC_Move_equal(const HC_Move *this, const HC_Move *that);

#ifdef __cplusplus
}
#endif

#endif
