#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "board.h"
#include "common.h"
#include "util.h"
#include "piece.h"

HC_Board * HC_Board_create()
{
	HC_Board *board = malloc(sizeof(HC_Board));
	board->da_pieces = DA_hc_pieces_create(8);
	board->en_passant_possible = 0;

	return board;
}

void HC_Board_destroy(HC_Board *board)
{
	DA_hc_pieces_destroy(board->da_pieces);
	free(board);
}

void HC_Board_copy(HC_Board *dest, HC_Board *src)
{
	HC_Board_clear(dest);
	DA_hc_pieces_append(dest->da_pieces, src->da_pieces);
	dest->en_passant_possible = src->en_passant_possible;
}

void HC_Board_clear(HC_Board *board)
{
	DA_hc_pieces_clear(board->da_pieces);
	board->en_passant_possible = 0;
}

int HC_Board_square_is_free(HC_Board *board, HC_Coordinates coords)
{
	for(size_t i = 0; i < board->da_pieces->size; i++)
	{
		if(HC_Coordinates_equal(board->da_pieces->data[i].coordinates, coords))
			return 0;
	}
	return 1;
}

/* Returns NULL if square is empty, else a pointer to the HC_Piece struct. */
HC_Piece *HC_Board_get_square(HC_Board *board, HC_Coordinates coords)
{
	for(size_t i = 0; i < board->da_pieces->size; i++)
	{
		if(HC_Coordinates_equal(board->da_pieces->data[i].coordinates, coords))
			return board->da_pieces->data + i;
	}

	return NULL;
}

int HC_Board_add_piece(HC_Board *board, HC_PieceType type, HC_Color color, HC_Coordinates coordinates, unsigned moved)
{
	/* Not allowed to add pieces where one is already present, also make sure to filter illegal coordinates. */
	if(!HC_Board_square_is_free(board, coordinates)
		|| !HC_Coordinates_valid(coordinates))
		return 0;

	HC_Piece piece = {.type = type, .color = color, .coordinates = coordinates, .moved = moved};
	DA_hc_pieces_push_back(board->da_pieces, &piece);

	return 1;
}

int HC_Board_remove_piece(HC_Board *board, HC_Coordinates coordinates)
{
	for(size_t i = 0; i < board->da_pieces->size; i++)
	{
		if(HC_Coordinates_equal(board->da_pieces->data[i].coordinates, coordinates))
		{
			DA_hc_pieces_erase(board->da_pieces, i, 1);
			return 1;
		}
	}

	return 0;
}

long int HC_Board_get_piece_index(HC_Board *board, HC_Piece *piece)
{
	for(size_t i = 0; i < board->da_pieces->size; i++)
	{
		if(board->da_pieces->data + i == piece)
		{
			return 1;
		}
	}

	return -1;
}

void HC_Board_default_position(HC_Board *board)
{
	HC_Board_clear(board);

	/* White pieces */
	HC_Coordinates coords = {.rank = 1, .file = 1};
	HC_Board_add_piece(board,	HC_ROOK,	HC_WHITE, coords, 0);
	coords.file++;
	HC_Board_add_piece(board,	HC_KNIGHT,	HC_WHITE, coords, 0);
	coords.file++;
	HC_Board_add_piece(board, 	HC_BISHOP,	HC_WHITE, coords, 0);
	coords.file++;
	HC_Board_add_piece(board,	HC_QUEEN,	HC_WHITE, coords, 0);
	coords.file++;
	HC_Board_add_piece(board,	HC_KING,	HC_WHITE, coords, 0);
	coords.file++;
	HC_Board_add_piece(board,	HC_BISHOP,	HC_WHITE, coords, 0);
	coords.file++;
	HC_Board_add_piece(board,	HC_KNIGHT,	HC_WHITE, coords, 0);
	coords.file++;
	HC_Board_add_piece(board,	HC_ROOK,	HC_WHITE, coords, 0);

	/* Black pieces */
	coords.rank = 8;
	HC_Board_add_piece(board,	HC_ROOK,	HC_BLACK, coords, 0);
	coords.file--;
	HC_Board_add_piece(board,	HC_KNIGHT,	HC_BLACK, coords, 0);
	coords.file--;
	HC_Board_add_piece(board, 	HC_BISHOP,	HC_BLACK, coords, 0);
	coords.file--;
	HC_Board_add_piece(board,	HC_KING,	HC_BLACK, coords, 0);
	coords.file--;
	HC_Board_add_piece(board,	HC_QUEEN,	HC_BLACK, coords, 0);
	coords.file--;
	HC_Board_add_piece(board,	HC_BISHOP,	HC_BLACK, coords, 0);
	coords.file--;
	HC_Board_add_piece(board,	HC_KNIGHT,	HC_BLACK, coords, 0);
	coords.file--;
	HC_Board_add_piece(board,	HC_ROOK,	HC_BLACK, coords, 0);

	/* White pawns */
	coords.rank = 2;
	while(coords.file <= 8)
	{
		HC_Board_add_piece(board, HC_PAWN,	HC_WHITE, coords, 0);
		coords.file++;
	}

	coords.file = 8;
	coords.rank = 7;
	while(coords.file >= 1)
	{
		HC_Board_add_piece(board, HC_PAWN,	HC_BLACK, coords, 0);
		coords.file--;
	}

	board->en_passant_possible = 0;
}

