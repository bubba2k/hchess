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
#### Functions

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
const HC_Move *HC_Game_get_legal_moves(HC_Game *game, int *n)
``` 
Returns a pointer to the first element of an array of the legal moves in the current position.
The number of legal moves in the array is put into @n.
Note: Do *not* try to mutate the array.


```c
HC_Color HC_Game_get_color_to_move(HC_Game *game)
```
Returns either `HC_WHITE` or `HC_BLACK`.

```c
const HC_Piece *HC_Game_get_pieces(HC_Game *game, int *n)
```
Returns a pointer to the first element of an array of the pieces on the board.
The number of pieces in the array is put into @n.
Note: Do *not* try to mutate the array.


```c
int HC_Game_play_move(HC_Game *game, HC_Move *move)
```
Play given move.
Returns 1 on success, or 0 if the passed move is not one of the legal moves in the current position.
Note: *Always* use one of the moves in the game's move list acquired with `HC_Game_get_legal_moves()`.
Creating and using your own move struct with this function will most likely result in undefined behaviour.


#### Structs

##### HC_Coordinates
 Member        |  Type       	|Purpose       
 :------------ | :--------   	| :-------------
 rank	       | unsigned int	| The rank.
 file           | unsigned int	 | The file.
 
 Note: Both rank and file range from 1 to 8. For the file, 1 equals file A and 8 equals file H etc.
 
##### HC_Move
 Member        |  Type       	|Purpose       
 :------------ | :--------   	| :-------------
 coords_from   | HC_Coordinates	| The coordinates of the moving piece.
 coords_to     | HC_Coordinates | The coordinates of the square the piece is moving to.
 
##### HC_Piece
 Member        	|  Type       		|Purpose       
 :------------ 	| :--------   		| :-------------
 type			| HC_PieceType		| The type of the piece.
 color			| HC_Color			| The color of the piece. Either `HC_WHITE` or `HC_BLACK`.
 coordinates   	| HC_Coordinates	| The coordinates of the piece.
				
Note: `type` is one of `HC_KING`, `HC_QUEEN`, `HC_ROOK`, `HC_BISHOP`, `HC_KNIGHT` or `HC_PAWN`.
