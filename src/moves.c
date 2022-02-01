#include <board.h>
#include <piece.h>
#include <common.h>
#include <moves.h>
#include <vision.h>
#include <dynamic_array_gen.h>

DYNAMIC_ARRAY_GEN_IMPL(HC_Move, hc_moves)

void HC_Piece_KING_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves);
void HC_Piece_QUEEN_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves);
void HC_Piece_BISHOP_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves);
void HC_Piece_ROOK_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves);
void HC_Piece_KNIGHT_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves);
void HC_Piece_PAWN_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves);

/* TODO: There is a lot of optimization to be done here, 
   especially considering some of this stuff is gonna
   end up in hot loops. */

int HC_Board_play_move(HC_Board *board, HC_Move *move)
{
	if(move->takes && !move->en_passant)
	{
		HC_Board_remove_piece(board, move->coords_to);
	}
	HC_Piece *piece = HC_Board_get_square(board, move->coords_from);
	piece->moved++;
	piece->coordinates = move->coords_to;

	/* Gotta move the rooks if castle */
	if(move->castle)
	{
		if(move->castle == 1) /* Short castle */
		{
			HC_Coordinates rook_coords = { .file = 8, .rank = 1 };
			if(piece->color == HC_BLACK)
				rook_coords.rank = 8;

			piece = HC_Board_get_square(board, rook_coords);
			rook_coords.file = 6;
			
			piece->coordinates = rook_coords;
		}
		if(move->castle == 2) /* Long castle */
		{
			HC_Coordinates rook_coords = { .file = 1, .rank = 1 };
			if(piece->color == HC_BLACK)
				rook_coords.rank = 8;

			piece = HC_Board_get_square(board, rook_coords);
			rook_coords.file = 4;
			
			piece->coordinates = rook_coords;
		}
	}

	/* NOTE: The pawn has already moved by this point. */
	piece = HC_Board_get_square(board, move->coords_to);
	if(move->en_passant)
	{	
		// Remove da taken pawn. 
		HC_Coordinates coord_to_take = move->coords_to;
		if(piece->color == HC_WHITE)
			coord_to_take.rank--;
		else
			coord_to_take.rank++;

		HC_Board_remove_piece(board, coord_to_take);
	}

	board->en_passant_possible = 0;
	piece = HC_Board_get_square(board, move->coords_to);
	if(piece->type == HC_PAWN)
	{
		/* Promotion. TODO: Currently only promotion to queen is possible */
		if(    (piece->color == HC_WHITE && piece->coordinates.rank == 8) 
			|| (piece->color == HC_BLACK && piece->coordinates.rank == 1) )
			piece->type = HC_QUEEN;

		/* If double advance happened */
		else if(piece->color == HC_WHITE && move->coords_from.rank + 2 == move->coords_to.rank)
		{
			board->en_passant_possible = move->coords_to.file;
		}
		else if(piece->color == HC_BLACK && move->coords_from.rank - 2 == move->coords_to.rank)
		{
			board->en_passant_possible = move->coords_to.file;
		}
	}

	return 1;
}

/* Check if the move @move is legal */
int HC_Move_legal(HC_Board *board, HC_Move *move)
{
	int legal;
	HC_Board *test_board = HC_Board_create();
	HC_Board_copy(test_board, board);

	HC_Color color = HC_Board_get_square(board, move->coords_from)->color;
	HC_Board_play_move(test_board, move);
	legal = !HC_player_in_check(test_board, color);

	HC_Board_destroy(test_board);
	return legal;
}

int HC_Move_equal(const HC_Move *this, const HC_Move *that)
{
	return (   HC_Coordinates_equal(this->coords_from, that->coords_from)
			&& HC_Coordinates_equal(this->coords_to,   that->coords_to));
}

/* more optimized for internal use */
int HC_Move_legal_opt(HC_Board *board, HC_Move *move, HC_Board *test_board, DA_hc_coords *enemy_vision)
{
	int legal;
	HC_Board_copy(test_board, board);

	HC_Color color = HC_Board_get_square(board, move->coords_from)->color;
	HC_Board_play_move(test_board, move);
	DA_hc_coords_clear(enemy_vision);
	HC_player_append_vision(test_board, !color, enemy_vision);

	legal = !HC_player_in_check_opt(test_board, color, enemy_vision);

	return legal;
}

/* Find all legal moves for a given player */
int HC_player_find_moves(HC_Board *board, HC_Color color, DA_hc_moves *moves)
{
	DA_hc_moves_clear(moves);

	for(size_t i = 0; i < board->da_pieces->size; i++)
	{
		HC_Piece *piece = board->da_pieces->data + i;
		
		if(piece->color == color)
		{
			HC_Piece_append_moves_unsafe(board, piece, moves);
		}
	}

	/* Discard all illegal moves */	
	DA_hc_coords *coords = DA_hc_coords_create(128);
	HC_Board *test_board = HC_Board_create();

	for(long int i = 0; i < (long int) moves->size && i >= 0; i++)
	{
		if(!HC_Move_legal_opt(board, moves->data + i, test_board, coords))
		{
			DA_hc_moves_erase(moves, i--, 1);
		}
	}

	DA_hc_coords_destroy(coords);
	HC_Board_destroy(test_board);

	return moves->size;
}

/* Find the moves for the given piece, and append them
   to the list. Returns the number of moves generated, or -1 if the piece
   is *not* on the given board.  */
int HC_Piece_find_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves)
{
	/* Check if the piece is actually on the given board */
	int on_board = 0;
	for(size_t i = 0; i < board->da_pieces->size; i++)
	{
		if(piece == board->da_pieces->data + i)
			on_board = 1;
	}
	if(!on_board)
		return -1;

	switch(piece->type)
	{
		case HC_KING:
			HC_Piece_KING_append_moves(board, piece, moves);
			break;
		case HC_QUEEN:
			HC_Piece_QUEEN_append_moves(board, piece, moves);
			break;
		case HC_ROOK:
			HC_Piece_ROOK_append_moves(board, piece, moves);
			break;
		case HC_BISHOP:
			HC_Piece_BISHOP_append_moves(board, piece, moves);
			break;
		case HC_KNIGHT:
			HC_Piece_KNIGHT_append_moves(board, piece, moves);
			break;
		case HC_PAWN:
			HC_Piece_PAWN_append_moves(board, piece, moves);
			break;
	}

	/* Discard all illegal moves */	
	DA_hc_coords *coords = DA_hc_coords_create(128);
	HC_Board *test_board = HC_Board_create();

	for(long int i = 0; i < (long int) moves->size && i >= 0; i++)
	{
		if(!HC_Move_legal_opt(board, moves->data + i, test_board, coords))
		{
			DA_hc_moves_erase(moves, i--, 1);
		}
	}

	DA_hc_coords_destroy(coords);
	HC_Board_destroy(test_board);

	return moves->size;
}

/* Similiar as 'HC_Piece_find_moves', but doesnt check whether the piece is actually on
   the board. So make sure you know what you are doing when using this function!
   Also does not clear the MoveList before appending new moves. */
void HC_Piece_append_moves_unsafe(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves)
{
	switch(piece->type)
	{
		case HC_KING:
			HC_Piece_KING_append_moves(board, piece, moves);
			break;
		case HC_QUEEN:
			HC_Piece_QUEEN_append_moves(board, piece, moves);
			break;
		case HC_ROOK:
			HC_Piece_ROOK_append_moves(board, piece, moves);
			break;
		case HC_BISHOP:
			HC_Piece_BISHOP_append_moves(board, piece, moves);
			break;
		case HC_KNIGHT:
			HC_Piece_KNIGHT_append_moves(board, piece, moves);
			break;
		case HC_PAWN:
			HC_Piece_PAWN_append_moves(board, piece, moves);
			break;
	}

	/* Discard all illegal moves */	
	DA_hc_coords *coords = DA_hc_coords_create(128);
	HC_Board *test_board = HC_Board_create();

	for(long int i = 0; i < (long int) moves->size && i >= 0; i++)
	{
		if(!HC_Move_legal_opt(board, moves->data + i, test_board, coords))
		{
			DA_hc_moves_erase(moves, i--, 1);
		}
	}

	DA_hc_coords_destroy(coords);
	HC_Board_destroy(test_board);
}

void HC_Piece_KING_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves)
{
	DA_hc_coords *vision = HC_Piece_get_vision(piece, board);
	DA_hc_coords *enemy_vision = HC_player_get_vision(board, !piece->color);

	for(unsigned i = 0; i < vision->size; i++)
	{
		HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
						 .check = 0, .castle = 0, .en_passant = 0 		};

		/* Check if the candidate square is under attack by the enemy */
		int square_attacked = 0;
		for(unsigned int j = 0; j < enemy_vision->size; j++)
		{
			if(HC_Coordinates_equal(enemy_vision->data[j], vision->data[i]))
			{
				square_attacked = 1;
				break;
			}
		}

		HC_Piece *target_piece = HC_Board_get_square(board, vision->data[i]);
		if(target_piece == NULL)
		{
			move.coords_to = vision->data[i];
			DA_hc_moves_push_back(moves, &move);
		}
		else
		{
			if(target_piece->color == piece->color || square_attacked)
				continue;

			move.coords_to = vision->data[i];
			move.takes = 1;

			DA_hc_moves_push_back(moves, &move);
		}
	}

	HC_Color playing_color = piece->color;
	if(HC_Board_short_castle_possible(board, playing_color))
	{
		HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
						 .check = 0, .castle = 1, .en_passant = 0 		};

		if(playing_color == HC_WHITE)
			move.coords_to.rank = 1;
		else // color == HC_BLACK
			move.coords_to.rank = 8;

		move.coords_to.file = 7;

		DA_hc_moves_push_back(moves, &move);
	}
	if(HC_Board_long_castle_possible(board, playing_color))
	{
		HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
						 .check = 0, .castle = 2, .en_passant = 0 		};
								/* 2 for long castle */
		if(playing_color == HC_WHITE)
			move.coords_to.rank = 1;
		else // color == HC_BLACK
			move.coords_to.rank = 8;

		move.coords_to.file = 3;

		DA_hc_moves_push_back(moves, &move);
	}
	
	DA_hc_coords_destroy(vision);
	DA_hc_coords_destroy(enemy_vision);
}

void HC_Piece_QUEEN_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves)
{
	DA_hc_coords *vision = HC_Piece_get_vision(piece, board);

	for(unsigned int i = 0; i < vision->size; i++)
	{
		HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
						 .check = 0, .castle = 0, .en_passant = 0 		};

		HC_Piece *target_piece = HC_Board_get_square(board, vision->data[i]);

		if(target_piece == NULL)
		{
			move.coords_to = vision->data[i];
			DA_hc_moves_push_back(moves, &move);
		}
		else
		{
			if(target_piece->color == piece->color)
				continue;

			move.coords_to = vision->data[i];
			move.takes     = 1;
			move.coords_to = target_piece->coordinates;
			DA_hc_moves_push_back(moves, &move);
		}
	}

	DA_hc_coords_destroy(vision);
}

void HC_Piece_ROOK_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves)
{
	DA_hc_coords *vision = HC_Piece_get_vision(piece, board);

	for(unsigned int i = 0; i < vision->size; i++)
	{
		HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
						 .check = 0, .castle = 0, .en_passant = 0 		};

		HC_Piece *target_piece = HC_Board_get_square(board, vision->data[i]);

		if(target_piece == NULL)
		{
			move.coords_to = vision->data[i];
			DA_hc_moves_push_back(moves, &move);
		}
		else
		{
			if(target_piece->color == piece->color)
				continue;

			move.coords_to = vision->data[i];
			move.takes     = 1;
			move.coords_to = target_piece->coordinates;
			DA_hc_moves_push_back(moves, &move);
		}
	}

	DA_hc_coords_destroy(vision);
}

void HC_Piece_BISHOP_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves)
{
	DA_hc_coords *vision = HC_Piece_get_vision(piece, board);

	for(unsigned int i = 0; i < vision->size; i++)
	{
		HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
						 .check = 0, .castle = 0, .en_passant = 0 		};

		HC_Piece *target_piece = HC_Board_get_square(board, vision->data[i]);

		if(target_piece == NULL)
		{
			move.coords_to = vision->data[i];
			DA_hc_moves_push_back(moves, &move);
		}
		else
		{
			if(target_piece->color == piece->color)
				continue;

			move.coords_to = vision->data[i];
			move.takes     = 1;
			move.coords_to = target_piece->coordinates;
			DA_hc_moves_push_back(moves, &move);
		}
	}

	DA_hc_coords_destroy(vision);
}


void HC_Piece_KNIGHT_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves)
{
	DA_hc_coords *vision = HC_Piece_get_vision(piece, board);

	for(unsigned int i = 0; i < vision->size; i++)
	{
		HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
						 .check = 0, .castle = 0, .en_passant = 0 		};

		HC_Piece *target_piece = HC_Board_get_square(board, vision->data[i]);

		if(target_piece == NULL)
		{
			move.coords_to = vision->data[i];
			DA_hc_moves_push_back(moves, &move);
		}
		else
		{
			if(target_piece->color == piece->color)
				continue;

			move.coords_to = vision->data[i];
			move.takes     = 1;
			move.coords_to = target_piece->coordinates;
			DA_hc_moves_push_back(moves, &move);
		}
	}

	DA_hc_coords_destroy(vision);
}

void HC_Piece_PAWN_append_moves(HC_Board *board, HC_Piece *piece, DA_hc_moves *moves)
{
	DA_hc_coords *vision = HC_Piece_get_vision(piece, board);

	for(unsigned int i = 0; i < vision->size; i++)
	{
		HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
						 .check = 0, .castle = 0, .en_passant = 0 		};

		HC_Piece *target_piece = HC_Board_get_square(board, vision->data[i]);

		if(target_piece == NULL)
		{
			continue;
		}
		else /* Moves with take */
		{
			if(target_piece->color == piece->color)
				continue;

			move.coords_to = vision->data[i];
			move.takes     = 1;
			move.coords_to = target_piece->coordinates;
			DA_hc_moves_push_back(moves, &move);
		}
	}

	/* Non attacking moves */
	/* The initial double advance */
	if(piece->color == HC_WHITE && piece->coordinates.rank == 2 && piece->moved == 0)
	{
		/* Make sure the destination is free */
		HC_Coordinates coords = { .rank = 4, .file = piece->coordinates.file };
		/* Also make sure there arent any pieces blocking the path */
		HC_Coordinates coords2 = { .rank = 3, .file = piece->coordinates.file };
		if(HC_Board_get_square(board, coords) == NULL && HC_Board_get_square(board, coords2) == NULL)
		{
			HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
							 .check = 0, .castle = 0, .en_passant = 0,
							 .coords_to = coords					 		};

			DA_hc_moves_push_back(moves, &move);
		}
	}
	else if(piece->color == HC_BLACK && piece->coordinates.rank == 7 && piece->moved == 0)
	{
		HC_Coordinates coords = { .rank = 5, .file = piece->coordinates.file };
		HC_Coordinates coords2 = { .rank = 6, .file = piece->coordinates.file };
		if(HC_Board_get_square(board, coords) == NULL && HC_Board_get_square(board, coords2) == NULL)
		{
			HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
							 .check = 0, .castle = 0, .en_passant = 0,
							 .coords_to = coords					 		};

			DA_hc_moves_push_back(moves, &move);
		}
	}

	/* Simple advance */
	if(piece->color == HC_WHITE)
	{
		HC_Coordinates coords  = { .rank = piece->coordinates.rank + 1, .file = piece->coordinates.file };
		if(HC_Coordinates_valid(coords) && HC_Board_get_square(board, coords) == NULL)
		{
			HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
							 .check = 0, .castle = 0, .en_passant = 0,
							 .coords_to = coords					 		};

			DA_hc_moves_push_back(moves, &move);
		}
	}
	else if(piece->color == HC_BLACK)
	{
		HC_Coordinates coords = { .rank = piece->coordinates.rank - 1, .file = piece->coordinates.file };
		if(HC_Coordinates_valid(coords) && HC_Board_get_square(board, coords) == NULL)
		{
			HC_Move move = { .coords_from = piece->coordinates, .takes = 0, 
							 .check = 0, .castle = 0, .en_passant = 0,
							 .coords_to = coords					 		};

			DA_hc_moves_push_back(moves, &move);
		}
	}

	/* En passant */
	HC_Coordinates pawn_coords = piece->coordinates;
	if(board->en_passant_possible)
	{
		if(    pawn_coords.file - 1 == board->en_passant_possible 
			|| pawn_coords.file + 1 == board->en_passant_possible )
		{
			if(piece->color == HC_WHITE)
			{
				if(piece->coordinates.rank == 5)
				{
					HC_Coordinates coords = { .file = board->en_passant_possible,
											  .rank = pawn_coords.rank + 1        };

					HC_Move move = { .coords_from = piece->coordinates, .takes = 1, 
							 		 .check = 0, .castle = 0, .en_passant = 1,
							 		 .coords_to = coords					 	  };

					DA_hc_moves_push_back(moves, &move);
				}
			}
			else // piece->color == BLACK
			{
				if(piece->coordinates.rank == 4)
				{
					HC_Coordinates coords = { .file = board->en_passant_possible,
											  .rank = pawn_coords.rank - 1        };

					HC_Move move = { .coords_from = piece->coordinates, .takes = 1, 
							 		 .check = 0, .castle = 0, .en_passant = 1,
							 		 .coords_to = coords					 	  };

					DA_hc_moves_push_back(moves, &move);
				}
			}
		}
	}

	DA_hc_coords_destroy(vision);
}

char *HC_Move_get_verbose_str(HC_Board *board, HC_Move *move)
{
	char *str = malloc(128);
	str[0] = '\0';

	/* Get moving piece */
	HC_Piece *moving_piece = HC_Board_get_square(board, move->coords_from);
	char moving_piece_str[32];
	HC_Piece_get_str(moving_piece, moving_piece_str, 32);
	strcat(str, moving_piece_str);

	HC_Piece *captured_piece = HC_Board_get_square(board, move->coords_to);
	if(captured_piece != NULL)
	{
		strcat(str, " taking ");
		char captured_piece_str[32];
		HC_Piece_get_str(captured_piece, captured_piece_str, 32);
		strcat(str, captured_piece_str);
	}
	else
	{
		strcat(str, " to ");
		char coords_str[3];
		HC_Coordinates_get_str(move->coords_to, coords_str, 3);
		strcat(str, coords_str);
	}

	return str;
}
