# hchess

hchess is a small library written in C99 implementing the chess game.
All rules except for threefold/fivefold and promotion (currently only promotion to queen is possible) are implemented.
The library is designed to allow you to get a game of chess up and running as quick as possible. Note that hchess does
not implement any IO.

### Building

If you are on a UNIX system, simply execute
`make static` or `make dynamic`
to build a static or dynamic library respectively.


Alternatively, compile all the files in `src` directly with your program and make sure to 
add the headers in `include` to your include path.


### Integration
Simply link your program with the library built as described above and include the `hchess.h` header in your code.
A simple example:
```c
#include <hchess.h>

int main()
{
	/* Initialize our chess game. */
	HC_Game *game = HC_Game_create();
	
	/* Game loop */
	HC_GameState state = HC_PLAYING;
	while(state == HC_PLAYING)
	{
		/* Get the array of pieces on the board */
		int n_pieces;
		HC_Piece *HC_Game_get_pieces(game, &n_pieces);
		
		/* Your code to draw the board here ... */
	
		/* Get the array of legal moves. */
		int n_legal_moves;
		HC_Move *legal_moves = HC_Game_get_legal_moves(game, &n_legal_moves);
		
		HC_Move *selected_move;
		/* Your code to select the desired move here ... */
		
		/* Play the selected move */
		HC_Game_play_move(game, selected_move);
		
		/* Update game state */
		state = HC_Game_get_state();
	}
	
	/* Display checkmate/draw/stalemate 
	 * switch(state) ...                  */
	
	return 0;
}
```

### API

All functions are in the HC namespace and act on a HC_Game object. 


```c 
HC_Game *HC_Game_create()
``` 
Initializes a game.


```c 
void HC_Game_destroy(HC_Game *game)
```
Destroy and free a game.


```c
void HC_Game_reset(HC_Game *game)
```
Reset a game to the default starting position.


```c
HC_GameState HC_Game_get_state(HC_Game *game)
```

Checks whether game has ended and returns a value of enum HC_GameState.
Possible return values are:
* `HC_PLAYING` - The game is on-going.
* `HC_BLACK_WINS` - Black has won the game by checkmate.
* `HC_WHITE_WINS` - White has won the game by checkmate.
* `HC_STALEMATE` - The game is a stalemate.
* `HC_DRAW_INSUFFICIENT_MATERIAL` - The game is a draw due to insufficient material.
* `HC_DRAW_50_NOVE_RULE` - The game is a draw due to the fifty move rule.


```c
HC_Move *HC_Game_get_legal_moves(HC_Game *game, int *n)
