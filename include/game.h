#ifndef HC_GAME_INCLUDED
#define HC_GAME_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "util.h"
#include "moves.h"

typedef struct HC_Game
{
	HC_Board *board;
	DA_hc_moves *legal_moves;
	DA_hc_moves *played_moves;
	
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

/* Create a game. */
HC_Game *HC_Game_create();

/* Destroy a game. */
void HC_Game_destroy(HC_Game *game);

/* Get the state of a game. This should be checked once every time
 * a move has been played. */
HC_GameState HC_Game_get_state(HC_Game *game);

/* Reset the game to starting position. */
void HC_Game_reset(HC_Game *game);

/* Play the given @move. Returns 1 on succes, or 0 if the 
 * move is corrupt/not one of the legal moves. */
int HC_Game_play_move(HC_Game *game, HC_Move *move);

/* Get an array of the legal moves. The number of legal moves
 * is put into @size. */
const HC_Move *HC_Game_get_legal_moves(HC_Game *game, int *size);

/* Check whose colors turn it is. */
HC_Color HC_Game_get_color_to_move(HC_Game *game);

/* Get a list of all pieces on the board. */
const HC_Piece *HC_Game_get_pieces(HC_Game *game, int *n);

#ifdef __cplusplus
}
#endif

#endif
