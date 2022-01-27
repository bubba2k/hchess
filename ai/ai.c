#include "ai.h"
#include "../board.h"
#include "../util.h"
#include "../moves.h"
#include "../game.h"

/* void ai_move(HC_Game *game); */

float HC_Board_shannon_eval(HC_Board *board, HC_Color color)
{
	/* Get the material values */
	int      num_queens	[2] = { 0, 0 },
			 num_knights[2] = { 0, 0 },
			 num_bishops[2] = { 0, 0 },
			 num_pawns	[2] = { 0, 0 },
			 num_rooks	[2] = { 0, 0 };
			
	for(unsigned int i = 0; i < board->da_pieces->size; i++)
	{
		HC_Color color = board->da_pieces->data[i].color;
		switch(board->da_pieces->data[i].type)
		{
			case HC_QUEEN:
				num_queens[color]	+= 1;
				break;
			case HC_ROOK:
				num_rooks[color] 	+= 1;
				break;
			case HC_KNIGHT:
				num_knights[color] 	+= 1;
				break;
			case HC_BISHOP:
				num_bishops[color] 	+= 1;
				break;
			case HC_PAWN:
				num_pawns[color] 	+= 1;
				break;
			default:
				break;
		}
	}
	
	/* Material evaluation */
	float material_eval =  (float)(num_queens[color]  - num_queens[!color] ) * 9.0f
						 + (float)(num_rooks[color]   - num_rooks[!color]  ) * 5.0f
						 + (float)(num_bishops[color] - num_bishops[!color]) * 3.0f
						 + (float)(num_knights[color] - num_knights[!color]) * 3.0f
						 + (float)(num_pawns[color]   - num_pawns[!color]  ) * 1.0f;

	/* Pawn structure evauluation */
	float pawn_eval	    = (   (HC_Board_get_num_blocked_pawns(board, color) 
						   	  - HC_Board_get_num_blocked_pawns(board, !color))
						   +  (HC_Board_get_num_isolated_pawns(board, color) 
						   	  - HC_Board_get_num_isolated_pawns(board, !color))
						   +  (HC_Board_get_num_doubled_pawns(board, color) 
						   	  - HC_Board_get_num_doubled_pawns(board, !color)))
						   * 0.5f;


	/* Mobility eval */
	DA_hc_moves *moves = DA_hc_moves_create(50);
	HC_player_find_moves(board, color, moves);
	int color_mobility = moves->size;
	DA_hc_moves_clear(moves);

	HC_player_find_moves(board, !color, moves);
	int other_color_mobility = moves->size;
	DA_hc_moves_destroy(moves);

	float mobility_eval = (float)(color_mobility - other_color_mobility) * 0.1f;

	/* Combine it */
	float eval = material_eval - pawn_eval + mobility_eval;

	return eval;
}

