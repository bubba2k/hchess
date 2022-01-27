#include "util.h"
#include "board.h"
#include "piece.h"
#include "common.h"
#include "vision.h"

void HC_Piece_KING_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords);
void HC_Piece_QUEEN_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords);
void HC_Piece_BISHOP_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords);
void HC_Piece_KNIGHT_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords);
void HC_Piece_PAWN_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords);
void HC_Piece_ROOK_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords);

void HC_Piece_append_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords);

DA_hc_coords *HC_player_get_vision(HC_Board *board, HC_Color color)
{
	DA_hc_coords *vision = DA_hc_coords_create(32);
	
	for(unsigned int i = 0; i < board->da_pieces->size; i++)
	{
		if(board->da_pieces->data[i].color == color)
		{
			HC_Piece_append_vision(board->da_pieces->data + i, board, vision);
		}
	}

	return vision;
}

void HC_player_append_vision(HC_Board *board, HC_Color color, DA_hc_coords *vision)
{
	for(unsigned int i = 0; i < board->da_pieces->size; i++)
	{
		if(board->da_pieces->data[i].color == color)
		{
			HC_Piece_append_vision(board->da_pieces->data + i, board, vision);
		}
	}
}

int HC_player_in_check(HC_Board *board, HC_Color color)
{
	/* Find the king */
	int return_val = 0;
	HC_Piece *king = NULL;
	for(unsigned int i = 0; i < board->da_pieces->size; i++)
	{
		HC_Piece *piece = board->da_pieces->data + i;
		if(piece->type == HC_KING && piece->color == color)
		{
			king = piece;
			break;
		}
	}
	if(king == NULL)
		return return_val;


	DA_hc_coords *vision = HC_player_get_vision(board, !color);
	for(unsigned int i = 0; i < vision->size; i++)
	{
		if(HC_Coordinates_equal(vision->data[i], king->coordinates))
		{
			return_val = 1;
			break;
		}
	}

	DA_hc_coords_destroy(vision);
	return return_val;
}

/* A more optimized version */
int HC_player_in_check_opt(HC_Board *board, HC_Color color, DA_hc_coords *enemy_vision)
{
	/* Find the king */
	int return_val = 0;
	HC_Piece *king = NULL;
	for(unsigned int i = 0; i < board->da_pieces->size; i++)
	{
		HC_Piece *piece = board->da_pieces->data + i;
		if(piece->type == HC_KING && piece->color == color)
		{
			king = piece;
			break;
		}
	}
	if(king == NULL)
		return return_val;

	for(unsigned int i = 0; i < enemy_vision->size; i++)
	{
		if(HC_Coordinates_equal(enemy_vision->data[i], king->coordinates))
		{
			return_val = 1;
			break;
		}
	}

	return return_val;
}

DA_hc_coords *HC_Piece_get_vision(HC_Piece *piece, HC_Board *board)
{
	DA_hc_coords *da_coords = DA_hc_coords_create(128);

	switch(piece->type)
	{
		case HC_KING:
			HC_Piece_KING_get_vision(piece, board, da_coords);
			break;
		case HC_QUEEN:
			HC_Piece_QUEEN_get_vision(piece, board, da_coords);
			break;
		case HC_BISHOP:
			HC_Piece_BISHOP_get_vision(piece, board, da_coords);
			break;
		case HC_KNIGHT:
			HC_Piece_KNIGHT_get_vision(piece, board, da_coords);
			break;
		case HC_PAWN:
			HC_Piece_PAWN_get_vision(piece, board, da_coords);
			break;
		case HC_ROOK:
			HC_Piece_ROOK_get_vision(piece, board, da_coords);
	}

	return da_coords;
}

void HC_Piece_append_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords)
{
	switch(piece->type)
	{
		case HC_KING:
			HC_Piece_KING_get_vision(piece, board, da_coords);
			break;
		case HC_QUEEN:
			HC_Piece_QUEEN_get_vision(piece, board, da_coords);
			break;
		case HC_BISHOP:
			HC_Piece_BISHOP_get_vision(piece, board, da_coords);
			break;
		case HC_KNIGHT:
			HC_Piece_KNIGHT_get_vision(piece, board, da_coords);
			break;
		case HC_PAWN:
			HC_Piece_PAWN_get_vision(piece, board, da_coords);
			break;
		case HC_ROOK:
			HC_Piece_ROOK_get_vision(piece, board, da_coords);
	}
}

void HC_Piece_KING_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords)
{
	board = board;

	/* Get all the squares adjacent to the king that are on the board. */
	for(unsigned int file = piece->coordinates.file - 1; file <= piece->coordinates.file + 1; file++)
	{
		HC_Coordinates coords = { .file = file, .rank = piece->coordinates.rank + 1 };
		if(HC_Coordinates_valid(coords))
			DA_hc_coords_push_back(da_coords, &coords);
	}
	for(unsigned int file = piece->coordinates.file - 1; file <= piece->coordinates.file + 1; file++)
	{
		HC_Coordinates coords = { .file = file, .rank = piece->coordinates.rank - 1 };
		if(HC_Coordinates_valid(coords))
			DA_hc_coords_push_back(da_coords, &coords);
	}
	{
		HC_Coordinates coords = { .file = piece->coordinates.file - 1, .rank  = piece->coordinates.rank };
		if(HC_Coordinates_valid(coords))
			DA_hc_coords_push_back(da_coords, &coords);
	}
	{
		HC_Coordinates coords = { .file = piece->coordinates.file + 1, .rank  = piece->coordinates.rank };
		if(HC_Coordinates_valid(coords))
			DA_hc_coords_push_back(da_coords, &coords);
	}
}

void HC_Piece_KNIGHT_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords)
{
	board = board;

	/* Find all the theoretical destination squares for the Knight. */
	HC_Coordinates coords;
	unsigned int p_rank  = piece->coordinates.rank;
	unsigned int p_file  = piece->coordinates.file;

	coords = (HC_Coordinates) { .rank = p_rank + 2, .file = p_file + 1 };
	if(HC_Coordinates_valid(coords))
		DA_hc_coords_push_back(da_coords, &coords);
	coords = (HC_Coordinates) { .rank = p_rank + 1, .file = p_file + 2 };
	if(HC_Coordinates_valid(coords))
		DA_hc_coords_push_back(da_coords, &coords);
	coords = (HC_Coordinates) { .rank = p_rank - 2, .file = p_file - 1 };
	if(HC_Coordinates_valid(coords))
		DA_hc_coords_push_back(da_coords, &coords);
	coords = (HC_Coordinates) { .rank = p_rank - 1, .file = p_file - 2 };
	if(HC_Coordinates_valid(coords))
		DA_hc_coords_push_back(da_coords, &coords);
	coords = (HC_Coordinates) { .rank = p_rank + 2, .file = p_file - 1 };
	if(HC_Coordinates_valid(coords))
		DA_hc_coords_push_back(da_coords, &coords);
	coords = (HC_Coordinates) { .rank = p_rank + 1, .file = p_file - 2 };
	if(HC_Coordinates_valid(coords))
		DA_hc_coords_push_back(da_coords, &coords);
	coords = (HC_Coordinates) { .rank = p_rank - 2, .file = p_file + 1 };
	if(HC_Coordinates_valid(coords))
		DA_hc_coords_push_back(da_coords, &coords);
	coords = (HC_Coordinates) { .rank = p_rank - 1, .file = p_file + 2 };
	if(HC_Coordinates_valid(coords))
		DA_hc_coords_push_back(da_coords, &coords);
}

void HC_Piece_BISHOP_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords)
{
	unsigned p_rank = piece->coordinates.rank, p_file = piece->coordinates.file;

	HC_Coordinates coords;

	for(coords.rank = p_rank + 1, coords.file = p_file + 1; HC_Coordinates_valid(coords); coords.rank++, coords.file++)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank + 1, coords.file = p_file - 1; HC_Coordinates_valid(coords); coords.rank++, coords.file--)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank - 1, coords.file = p_file - 1; HC_Coordinates_valid(coords); coords.rank--, coords.file--)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank - 1, coords.file = p_file + 1; HC_Coordinates_valid(coords); coords.rank--, coords.file++)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
}

void HC_Piece_ROOK_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords)
{
	unsigned p_rank = piece->coordinates.rank, p_file = piece->coordinates.file;

	HC_Coordinates coords;

	for(coords.rank = p_rank + 1, coords.file = p_file; HC_Coordinates_valid(coords); coords.rank++)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank - 1, coords.file = p_file; HC_Coordinates_valid(coords); coords.rank--)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank, coords.file = p_file + 1; HC_Coordinates_valid(coords); coords.file++)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank, coords.file = p_file - 1; HC_Coordinates_valid(coords); coords.file--)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
}

void HC_Piece_QUEEN_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords)
{
	unsigned p_rank = piece->coordinates.rank, p_file = piece->coordinates.file;

	HC_Coordinates coords;
	
	/* Diagonals */
	for(coords.rank = p_rank + 1, coords.file = p_file + 1; HC_Coordinates_valid(coords); coords.rank++, coords.file++)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank + 1, coords.file = p_file - 1; HC_Coordinates_valid(coords); coords.rank++, coords.file--)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank - 1, coords.file = p_file - 1; HC_Coordinates_valid(coords); coords.rank--, coords.file--)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank - 1, coords.file = p_file + 1; HC_Coordinates_valid(coords); coords.rank--, coords.file++)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}

	/* Files / Ranks */
	for(coords.rank = p_rank + 1, coords.file = p_file; HC_Coordinates_valid(coords); coords.rank++)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank - 1, coords.file = p_file; HC_Coordinates_valid(coords); coords.rank--)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank, coords.file = p_file + 1; HC_Coordinates_valid(coords); coords.file++)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
	for(coords.rank = p_rank, coords.file = p_file - 1; HC_Coordinates_valid(coords); coords.file--)
	{
		HC_Piece *other_piece = HC_Board_get_square(board, coords);

		DA_hc_coords_push_back(da_coords, &coords);

		if(other_piece != NULL)
			break;
	}
}

void HC_Piece_PAWN_get_vision(HC_Piece *piece, HC_Board *board, DA_hc_coords *da_coords)
{
	board = board;
	unsigned p_rank = piece->coordinates.rank, p_file = piece->coordinates.file;

	HC_Coordinates coords;
	if(piece->color == HC_WHITE)
	{
		coords = (HC_Coordinates) { .rank = p_rank + 1, .file = p_file + 1 };
		if(HC_Coordinates_valid(coords))
			DA_hc_coords_push_back(da_coords, &coords);
		coords = (HC_Coordinates) { .rank = p_rank + 1, .file = p_file - 1 };
		if(HC_Coordinates_valid(coords))
			DA_hc_coords_push_back(da_coords, &coords);
	}
	else
	{
		coords = (HC_Coordinates) { .rank = p_rank - 1, .file = p_file + 1 };
		if(HC_Coordinates_valid(coords))
			DA_hc_coords_push_back(da_coords, &coords);
		coords = (HC_Coordinates) { .rank = p_rank - 1, .file = p_file - 1 };
		if(HC_Coordinates_valid(coords))
			DA_hc_coords_push_back(da_coords, &coords);
	}
}
