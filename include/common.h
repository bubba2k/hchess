#ifndef HC_COMMON_H_INCLUDED
#define HC_COMMON_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "dynamic_array_gen.h"

typedef enum HC_PieceType { HC_KING = 0, HC_QUEEN = 1, HC_BISHOP = 2, HC_KNIGHT = 3, HC_ROOK = 4, HC_PAWN = 5 } HC_PieceType;
typedef enum HC_Color{ HC_BLACK = 0, HC_WHITE = 1 } HC_Color;

typedef struct { unsigned int rank, file; } HC_Coordinates;

DYNAMIC_ARRAY_GEN_DECL(HC_Coordinates, hc_coords)

#ifdef __cplusplus
}
#endif

#endif
