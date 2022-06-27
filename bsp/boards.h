#ifndef BOARDS_H__
#define BOARDS_H__

#if defined(BOARD_RASPBERRY4_V1_0)
    #include "board_raspberry4_v1.0.h"
#elif defined(BOARD_RASPBERRY4_V1_1)
    #include "board_raspberry4_v1.1.h"
#else
    #error "board is not defined"
#endif


#endif
