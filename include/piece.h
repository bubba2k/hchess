#ifndef HC_PIECE_INCLUDED
#define HC_PIECE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "common.h"
#include "dynamic_array_gen.h"

typedef struct Piece
{
	HC_PieceType type;
	HC_Color     color;
	HC_Coordinates coordinates;
	unsigned int moved;

} HC_Piece;

DYNAMIC_ARRAY_GEN_DECL(HC_Piece, hc_pieces)

void HC_Piece_get_str(HC_Piece *piece, char *buf, size_t buf_size);

#ifdef __cplusplus
}
#endif

#endif
