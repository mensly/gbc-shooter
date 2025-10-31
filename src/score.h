#ifndef SCORE_H
#define SCORE_H

#include <gb/gb.h>

void score_init(void);           // load top score from SRAM
void score_reset(void);          // reset current score to 0
void score_add(UINT8 amount);    // add to current score
UINT16 score_current(void);
UINT16 score_top(void);
void score_try_update_top(void); // save to SRAM if current > top

#endif

