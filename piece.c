#include "piece.h"
#include "board.h"
#include "common.h"
#include "util.h"
#include "dynamic_array_gen.h"

DYNAMIC_ARRAY_GEN_IMPL(HC_Piece, hc_pieces)

void HC_Piece_get_str(HC_Piece *piece, char *buf, size_t buf_size)
{
	char *type;
	switch(piece->type)
	{
		case HC_ROOK:
			type = "Rook";
			break;
		case HC_QUEEN:
			type = "Queen";
			break;
		case HC_KING:
			type = "King";
			break;
		case HC_BISHOP:
			type = "Bishop";
			break;
		case HC_KNIGHT:
			type = "Knight";
			break;
		case HC_PAWN:
			type = "Pawn";
			break;
	}

	char *color = piece->color == HC_WHITE ? "White" : "Black";

	char coords[3];
	HC_Coordinates_get_str(piece->coordinates, coords, 3);

	snprintf(buf, buf_size, "%s: %s %s %u", coords, color, type, piece->moved);
}
