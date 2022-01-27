#include "common.h"
#include "game.h"
#include "board.h"

HC_Game *HC_Game_create()
{
	HC_Game *game = malloc(sizeof(HC_Game));

	game->da_move_strings = DA_charptr_create(100);

	game->played_moves = DA_hc_moves_create(50);
	game->legal_moves = DA_hc_moves_create(50);

	game->board = HC_Board_create();
	HC_Board_default_position(game->board);

	game->move_played_since_move_gen = 1;
	game->num_moves_played = 0;
	game->moves_since_capture_or_pawn_move = 0;
	game->color_to_move = HC_WHITE;

	return game;
}

void HC_Game_reset(HC_Game *game)
{
	HC_Board_default_position(game->board);
	game->num_moves_played = 0;
	game->moves_since_capture_or_pawn_move = 0;
	game->move_played_since_move_gen = 1;
	game->color_to_move = HC_WHITE;

	DA_hc_moves_clear(game->played_moves);
	DA_hc_moves_clear(game->legal_moves);

	for(size_t i = 0; i < game->da_move_strings->size; i++)
	{
		free(game->da_move_strings->data[i]);
	}

	DA_charptr_clear(game->da_move_strings);
}

void HC_Game_destroy(HC_Game *game)
{
	DA_hc_moves_destroy(game->played_moves);
	DA_hc_moves_destroy(game->legal_moves);

	for(size_t i = 0; i < game->da_move_strings->size; i++)
	{
		free(game->da_move_strings->data[i]);
	}
	DA_charptr_destroy(game->da_move_strings);

	HC_Board_destroy(game->board);
	free(game);
}

int HC_Game_play_move(HC_Game *game, HC_Move *move)
{
	char *move_str = HC_Move_get_verbose_str(game->board, move);
	DA_charptr_push_back(game->da_move_strings, &move_str);

	if(move->takes && !move->en_passant)
	{
		HC_Board_remove_piece(game->board, move->coords_to);
	}
	HC_Piece *piece = HC_Board_get_square(game->board, move->coords_from);
	piece->moved++;
	piece->coordinates = move->coords_to;

	/* for the 50 move rule */
	if(!(piece->type == HC_PAWN ||  move->takes))
	{
		game->moves_since_capture_or_pawn_move++;
	}
	else
	{
		game->moves_since_capture_or_pawn_move = 0;
	}
	
	/* Gotta move the rooks if castle */
	if(move->castle)
	{
		if(move->castle == 1) /* Short castle */
		{
			HC_Coordinates rook_coords = { .file = 8, .rank = 1 };
			if(piece->color == HC_BLACK)
				rook_coords.rank = 8;

			piece = HC_Board_get_square(game->board, rook_coords);
			rook_coords.file = 6;
			
			piece->coordinates = rook_coords;
		}
		if(move->castle == 2) /* Long castle */
		{
			HC_Coordinates rook_coords = { .file = 1, .rank = 1 };
			if(piece->color == HC_BLACK)
				rook_coords.rank = 8;

			piece = HC_Board_get_square(game->board, rook_coords);
			rook_coords.file = 4;
			
			piece->coordinates = rook_coords;
		}
	}

	/* NOTE: The pawn has already moved by this point. */
	piece = HC_Board_get_square(game->board, move->coords_to);
	if(move->en_passant)
	{	
		// Remove da taken pawn. 
		HC_Coordinates coord_to_take = move->coords_to;
		if(piece->color == HC_WHITE)
			coord_to_take.rank--;
		else
			coord_to_take.rank++;

		HC_Board_remove_piece(game->board, coord_to_take);
	}

	game->board->en_passant_possible = 0;
	piece = HC_Board_get_square(game->board, move->coords_to);
	if(piece->type == HC_PAWN)
	{
		/* Promotion. TODO: Currently only promotion to queen is possible */
		if(    (piece->color == HC_WHITE && piece->coordinates.rank == 8) 
			|| (piece->color == HC_BLACK && piece->coordinates.rank == 1) )
			piece->type = HC_QUEEN;

		/* If double advance happened */
		else if(piece->color == HC_WHITE && move->coords_from.rank + 2 == move->coords_to.rank)
		{
			game->board->en_passant_possible = move->coords_to.file;
		}
		else if(piece->color == HC_BLACK && move->coords_from.rank - 2 == move->coords_to.rank)
		{
			game->board->en_passant_possible = move->coords_to.file;
		}
	}

	/* Flip the color */
	game->color_to_move = !game->color_to_move;

	/* Append the moves */
	game->num_moves_played++;

	DA_hc_moves_push_back(game->played_moves, move);

	game->move_played_since_move_gen = 1;

	return 1;
}

int HC_player_insufficient_material(HC_Board *board, HC_Color color)
{
	unsigned num_bishops = 0, num_knights = 0;

	for(size_t i = 0; i < board->da_pieces->size; i++)
	{	
		HC_Piece *piece = board->da_pieces->data + i;
		
		if(piece->color == color)
		{
			switch(piece->type)
			{
				case HC_QUEEN:
					return 0;
				case HC_ROOK:
					return 0;
				case HC_BISHOP:
					num_bishops++;
					break;
				case HC_KNIGHT:
					num_knights++;
					break;
				case HC_PAWN:
					return 0;
				default:
					break;
			}
		}
	}

	if(    (num_knights >= 1 && num_bishops >= 1)
		|| (num_knights >= 2 || num_bishops >= 2) )
		return 0;

	return 1;
}

HC_GameState HC_Game_get_state(HC_Game *game)
{
	HC_GameState state = HC_PLAYING;
	HC_Color playing_color = game->color_to_move;

	/* 50 Move rule */
	if(game->moves_since_capture_or_pawn_move >= 100)
		return HC_DRAW_50_MOVE_RULE;

	/* Insufficient material check */
	int white_insufficient = HC_player_insufficient_material(game->board, HC_WHITE);
	int black_insufficient = HC_player_insufficient_material(game->board, HC_BLACK);

	if(white_insufficient && black_insufficient)
		return HC_DRAW_INSUFFICIENT_MATERIAL;
	
	/* Checkmate or stalemate */
	DA_hc_moves *moves = DA_hc_moves_create(64);
	HC_player_find_moves(game->board, playing_color, moves);
	if(moves->size == 0)
	{
		if(HC_player_in_check(game->board, playing_color))
		{
			state = !playing_color;
		}
		else
		{
			state = HC_STALEMATE;
		}
	}

	DA_hc_moves_destroy(moves);

	return state;
}

HC_Move *HC_Game_get_legal_moves(HC_Game *game, int *size)
{
	if(game->move_played_since_move_gen)
		HC_player_find_moves(game->board, game->color_to_move, game->legal_moves);

	*size = game->legal_moves->size;
	return game->legal_moves->data;
}
