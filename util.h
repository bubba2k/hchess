#ifndef HC_UTIL_H_INCLUDED
#define HC_UTIL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "board.h"
#include "piece.h"

DYNAMIC_ARRAY_GEN_DECL(char *, charptr)

int HC_Coordinates_equal(HC_Coordinates coords1, HC_Coordinates coords2);
int HC_Coordinates_valid(HC_Coordinates coords);
int HC_Coordinates_get_str(HC_Coordinates coords, char *buf, size_t buf_size);

int HC_Board_short_castle_possible(HC_Board *board, HC_Color color);
int HC_Board_long_castle_possible(HC_Board *board, HC_Color color);


void HC_Board_print(HC_Board *board);
char *HC_Board_get_graphic_str(HC_Board *board);

int HC_Board_get_num_blocked_pawns(HC_Board *board, HC_Color color);
int HC_Board_get_num_doubled_pawns(HC_Board *board, HC_Color color);
int HC_Board_get_num_isolated_pawns(HC_Board *board, HC_Color color);

#ifdef __cplusplus
}
#endif

#endif
