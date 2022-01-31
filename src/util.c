#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <common.h>
#include <board.h>
#include <util.h>
#include <piece.h>
#include <vision.h>
#include <moves.h>

#include <dynamic_array_gen.h>

DYNAMIC_ARRAY_GEN_IMPL(char *, charptr)
DYNAMIC_ARRAY_GEN_IMPL(HC_Coordinates, hc_coords)

int HC_Coordinates_equal(HC_Coordinates coords1, HC_Coordinates coords2)
{
	return (coords1.rank == coords2.rank && coords1.file == coords2.file);
}

int HC_Coordinates_valid(HC_Coordinates coords)
{
	return (    (coords.file <= 8 && coords.file >= 1)
			&&  (coords.rank <= 8 && coords.rank >= 1) );
}

int HC_Board_get_num_isolated_pawns(HC_Board *board, HC_Color color)
{
	int num = 0;
	for(unsigned int i = 0; i < board->da_pieces->size; i++)
	{
		HC_Piece *piece = board->da_pieces->data + i;
		if(piece->type == HC_PAWN && piece->color == color)
		{
			/* Check if there is a pawn present on the adjacent files */
			HC_Coordinates coords = piece->coordinates;
			int isolated = 1;
			for(int i = -1; i <= 1; i+=2)
			{
				coords.file = i + piece->coordinates.file;
				for(unsigned rank = 1; rank <= 8; rank++)
				{
					coords.rank = rank;
					HC_Piece *piece = HC_Board_get_square(board, coords);
					if(piece != NULL && piece->color == color && piece->type == HC_PAWN)
					{
						isolated = 0;
						break;
					}
				}
			}

			num += isolated;
		}
	}

	return num;
}

int HC_Board_get_num_doubled_pawns(HC_Board *board, HC_Color color)
{
	int num = 0;
	for(unsigned int i = 0; i < board->da_pieces->size; i++)
	{
		HC_Piece *piece = board->da_pieces->data + i;
		if(piece->type == HC_PAWN && piece->color == color)
		{
			/* Check if there is a pawn present on the same files */
			int doubled = 0;

			HC_Coordinates coords = { .file = piece->coordinates.file };
			for(unsigned rank = 1; rank <= 8; rank++)
			{
				coords.rank = rank;
				HC_Piece *otherpiece = HC_Board_get_square(board, coords);
				if(    otherpiece != NULL && otherpiece->color == color && otherpiece->type == HC_PAWN
					&& !HC_Coordinates_equal(coords, piece->coordinates))
				{
					doubled = 1;
					break;
				}
			}

			num += doubled;
		}
	}

	return num;
}

int HC_Board_get_num_blocked_pawns(HC_Board *board, HC_Color color)
{
	int num = 0;
	for(unsigned int i = 0; i < board->da_pieces->size; i++)
	{
		HC_Piece *piece = board->da_pieces->data + i;
		if(piece->type == HC_PAWN && piece->color == color)
		{
			/* Check if the pawn is blocked */
			int blocked = 0;

			DA_hc_moves *pawn_moves = DA_hc_moves_create(4);
			HC_Piece_find_moves(board, piece, pawn_moves);
			if(pawn_moves->size == 0)
				blocked = 1;
			DA_hc_moves_destroy(pawn_moves);


			num += blocked;
		}
	}

	return num;
}

int HC_Coordinates_get_str(HC_Coordinates coords, char *buf, size_t buf_size)
{
	if(!HC_Coordinates_valid(coords))
	{
		snprintf(buf, buf_size, "XX");
		return 0;
	}

	char str[2];
	str[0] = 'A' - 1 + coords.file;
	str[1] = '0'     + coords.rank;

	snprintf(buf, buf_size, "%s", str);

	return 1;
}

void HC_Board_print(HC_Board *board)
{
	char str[256];

	printf("Board with %lu pieces:\n", board->da_pieces->size);
	for(size_t i = 0; i < board->da_pieces->size; i++)
	{
		HC_Piece_get_str(board->da_pieces->data + i, str, 256);
		printf("%s\n", str);
	}
}

int HC_Board_short_castle_possible(HC_Board *board, HC_Color color)
{
	HC_Piece *piece;
	HC_Coordinates coords;
	if(color == HC_WHITE)
		coords.rank = 1;
	else // color == HC_BLACK
		coords.rank  = 8;

	/* Check if the king is home */
	coords.file = 5;
	piece = HC_Board_get_square(board, coords);
	if( !(piece != NULL && piece->type == HC_KING 
		  && piece->color == color && piece->moved == 0) )
		return 0;

	/* Check if rook is home */
	coords.file = 8;
	piece = HC_Board_get_square(board, coords);
	if( !(piece != NULL && piece->type == HC_ROOK
			&& piece->color == color && piece->moved == 0))
		return 0;

	/* Check if the squares between rook and king are empty */
	HC_Piece *piece2;
	coords.file = 7;
	piece  = HC_Board_get_square(board, coords);
	coords.file = 6;
	piece2 = HC_Board_get_square(board, coords);
	if(piece != NULL || piece2 != NULL)
		return 0;

	/* Check if the squares between rook and king are not attacked */
	int attacked = 0;
	DA_hc_coords *vision = HC_player_get_vision(board, !color);
	coords.file = 7;
	for(size_t i = 0; i < vision->size; i++)
	{
		if(HC_Coordinates_equal(coords, vision->data[i]))
		{
			attacked = 1;
			break;
		}
	}
	coords.file = 6;
	for(size_t i = 0; i < vision->size && !attacked; i++)
	{
		if(HC_Coordinates_equal(coords, vision->data[i]))
		{
			attacked = 1;
			break;
		}
	}
	
	DA_hc_coords_destroy(vision);
	return !attacked;
}

int HC_Board_long_castle_possible(HC_Board *board, HC_Color color)
{
	HC_Piece *piece;
	HC_Coordinates coords;
	if(color == HC_WHITE)
		coords.rank = 1;
	else
		coords.rank  = 8;

	/* Check if the king is home */
	coords.file = 5;
	piece = HC_Board_get_square(board, coords);
	if( !(piece != NULL && piece->type == HC_KING 
		  && piece->color == color && piece->moved == 0) )
		return 0;

	/* Check if rook is home */
	coords.file = 1;
	piece = HC_Board_get_square(board, coords);
	if(!(piece != NULL && piece->type == HC_ROOK
			&& piece->color == color && piece->moved == 0))
		return 0;

	/* Check if the squares between rook and king are empty */
	HC_Piece *piece2, *piece3;
	coords.file = 2;
	piece  = HC_Board_get_square(board, coords);
	coords.file = 3;
	piece2 = HC_Board_get_square(board, coords);
	coords.file = 4;
	piece3 = HC_Board_get_square(board, coords);
	if(piece != NULL || piece2 != NULL || piece3 != NULL)
		return 0;

	/* Check if the squares between rook and king are not attacked */
	int attacked = 0;
	DA_hc_coords *vision = HC_player_get_vision(board, !color);
	coords.file = 3;
	for(size_t i = 0; i < vision->size; i++)
	{
		if(HC_Coordinates_equal(coords, vision->data[i]))
		{
			attacked = 1;
			break;
		}
	}
	coords.file = 4;
	for(size_t i = 0; i < vision->size && !attacked; i++)
	{
		if(HC_Coordinates_equal(coords, vision->data[i]))
		{
			attacked = 1;
			break;
		}
	}
	
	DA_hc_coords_destroy(vision);
	return !attacked;
}

char *HC_Board_get_graphic_str(HC_Board *board)
{
	char *str = calloc(128 + 10, sizeof(char));
	unsigned int index = 0;
	str[index++] = ' ';
	for(unsigned int rank = 8; rank >= 1; rank--)
	{
		for(unsigned int file = 1; file <= 9; file++)
		{
			if(file == 9)
			{
				str[index] = '\n';
			}
			else
			{
				HC_Coordinates coords = { .file = file, .rank = rank };
				HC_Piece *piece_ptr = HC_Board_get_square(board, coords);

				if(piece_ptr == NULL)
				{
					str[index] = '.';
				}
				else
				{
					char c;
					switch(piece_ptr->type)
					{
						case HC_KING:
							c = 'K';
							break;
						case HC_QUEEN:
							c = 'Q';
							break;
						case HC_BISHOP:
							c = 'B';
							break;
						case HC_KNIGHT:
							c = 'N';
							break;
						case HC_ROOK:
							c = 'R';
							break;
						case HC_PAWN:
							c = 'P';
							break;
					}
					// Black pieces are lower case
					if(piece_ptr->color == HC_BLACK)
						c += 'a' - 'A';

					str[index] = c;
				}
			}
			index++;
			str[index++] = ' ';

		}
	}

	return str;
}
