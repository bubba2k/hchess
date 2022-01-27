#ifndef HC_NOTATION_H_INCLUDED
#define HC_NOTATION_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "../board.h"
#include "../moves.h"
#include "../common.h"

int HC_Move_get_notation(HC_Board *board, HC_Move *move, char *buf, size_t buf_size);

#ifdef __cplusplus
}
#endif

#endif
