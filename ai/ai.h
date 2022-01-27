#ifndef HC_AI_H_INCLUDED
#define HC_AI_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../board.h"

float HC_Board_shannon_eval(HC_Board *board, HC_Color color);

#ifdef __cplusplus
}
#endif

#endif
