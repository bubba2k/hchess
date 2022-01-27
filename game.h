#ifndef HC_GAME_INCLUDED
#define HC_GAME_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "util.h"
#include "moves.h"
#include "vision.h"

typedef struct HC_Game
{
	HC_Board *board;
	DA_hc_moves *legal_moves;
	DA_hc_moves *played_moves;
	DA_charptr *da_move_strings;
	
	int move_played_since_move_gen;
	unsigned num_moves_played;
	unsigned moves_since_capture_or_pawn_move;
	HC_Color color_to_move;
} HC_Game;

typedef enum {	HC_BLACK_WINS = 0, HC_WHITE_WINS = 1, 
				HC_STALEMATE = 2, 
				HC_DRAW_INSUFFICIENT_MATERIAL = 3,
				HC_DRAW_50_MOVE_RULE = 4,
			    HC_PLAYING = 5
			 } HC_GameState;

HC_GameState HC_Game_get_state(HC_Game *game);
HC_Game *HC_Game_create();
void HC_Game_destroy(HC_Game *game);
void HC_Game_reset(HC_Game *game);
int HC_Game_play_move(HC_Game *game, HC_Move *move);
HC_Move *HC_Game_get_legal_moves(HC_Game *game, int *size);

#ifdef __cplusplus
}
#endif

#endif
