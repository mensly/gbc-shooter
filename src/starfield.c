#include <gb/gb.h>
#include <gb/cgb.h>
#include "starfield.h"

// Background tiles: 0=black, 1=tiny bright dot, 2=tiny dim dot
static const unsigned char bg_tiles[] = {
    // Tile 0: solid color 0 (black)
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    // Tile 1: ultra-tiny bright star (single pixel, color index 1)
    0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    // Tile 2: ultra-tiny dim star (single pixel, color index 2)
    0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

static UINT8 rng = 1;
static UINT8 rand8(void) { rng = (rng * 109u + 89u); return rng; }
static UINT8 scroll_y = 0;

void starfield_set_palettes(void) {
    // Background black + star colors
    BGP_REG = 0b00011011; // DMG
    if (_cpu == CGB_TYPE) {
        const palette_color_t bg0[] = {
            RGB(0,0,0), RGB(31,31,31), RGB(16,16,16), RGB(8,8,8)
        };
        set_bkg_palette(0, 1, bg0);
    }
}

static void starfield_fill_row(UINT8 row) {
    for (UINT8 x = 0; x < 20; x++) {
        set_bkg_tile_xy(x, row & 31u, 0);
    }
    UINT8 count = rand8() % 3; // 0..2 stars per row
    for (UINT8 i = 0; i < count; i++) {
        UINT8 sx = rand8() % 20;
        unsigned char t = (rand8() & 1) ? 1 : 2;
        set_bkg_tile_xy(sx, row & 31u, t);
    }
}

void starfield_init(void) {
    set_bkg_data(0, 3, bg_tiles);
    for (UINT8 y = 0; y < 32; y++) starfield_fill_row(y);
    scroll_y = 0;
    SCY_REG = scroll_y;
}

void starfield_update(void) {
    scroll_y--;
    SCY_REG = scroll_y;
    if ((scroll_y & 7u) == 0u) {
        UINT8 bottom_row = ((scroll_y >> 3) + 18u) & 31u;
        starfield_fill_row(bottom_row);
    }
}


