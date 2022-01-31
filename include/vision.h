#ifndef HC_VISION_H_INCLUDED
#define HC_VISION_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"
#include "board.h"
#include "piece.h"
#include "common.h"

DA_hc_coords *HC_Piece_get_vision(HC_Piece *piece, HC_Board *board);
DA_hc_coords *HC_player_get_vision(HC_Board *board, HC_Color);
void HC_player_append_vision(HC_Board *board, HC_Color color, DA_hc_coords *coords);

int HC_player_in_check(HC_Board *board, HC_Color color);
int HC_player_in_check_opt(HC_Board *board, HC_Color color, DA_hc_coords *enemy_vision);

#ifdef __cplusplus
}
#endif

#endif
