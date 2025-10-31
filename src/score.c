#include <gb/gb.h>
#include <gb/hardware.h>
#include "score.h"

static UINT16 g_current = 0;
static UINT16 g_top = 0;

// Simple SRAM layout at 0xA000: [sig 'S''S'][top_lo][top_hi][chk]
#define SRAM_PTR ((volatile UINT8 *)0xA000)

static UINT8 calc_chk(UINT8 lo, UINT8 hi) { return (UINT8)(0x5A ^ lo ^ hi); }

void score_init(void) {
    ENABLE_RAM_MBC1;
    SWITCH_RAM_MBC1(0);
    if (SRAM_PTR[0] == 'S' && SRAM_PTR[1] == 'S') {
        UINT8 lo = SRAM_PTR[2];
        UINT8 hi = SRAM_PTR[3];
        UINT8 chk = SRAM_PTR[4];
        if (chk == calc_chk(lo, hi)) {
            g_top = (UINT16)(lo | ((UINT16)hi << 8));
        }
    }
    DISABLE_RAM_MBC1;
}

void score_reset(void) { g_current = 0; }

void score_add(UINT8 amount) { g_current += amount; }

UINT16 score_current(void) { return g_current; }
UINT16 score_top(void) { return g_top; }

void score_try_update_top(void) {
    if (g_current > g_top) {
        g_top = g_current;
        ENABLE_RAM_MBC1;
        SWITCH_RAM_MBC1(0);
        UINT8 lo = (UINT8)(g_top & 0xFF);
        UINT8 hi = (UINT8)((g_top >> 8) & 0xFF);
        SRAM_PTR[0] = 'S';
        SRAM_PTR[1] = 'S';
        SRAM_PTR[2] = lo;
        SRAM_PTR[3] = hi;
        SRAM_PTR[4] = calc_chk(lo, hi);
        DISABLE_RAM_MBC1;
    }
}


