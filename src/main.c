#include <gb/gb.h>
#include <gb/cgb.h>
#include "graphics.h"
#include "entities.h"

static void init_palettes(void) {
    // Background black for space
    BGP_REG = 0b00011011; // DMG: 00=black, 01=dark, 10=light, 11=white
    if (_cpu == CGB_TYPE) {
        const palette_color_t bg0[] = {
            RGB(0,0,0),     // color 0: black (background)
            RGB(31,31,31),  // color 1: bright star
            RGB(16,16,16),  // color 2: dim star
            RGB(8,8,8)      // color 3: unused
        };
        set_bkg_palette(0, 1, bg0);
    }
}

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

static UINT8 scroll_y = 0; // in pixels (0..255 wraps)

static void starfield_fill_row(UINT8 row) {
    // Clear row to black first
    for (UINT8 x = 0; x < 20; x++) {
        set_bkg_tile_xy(x, row & 31u, 0);
    }
    // Place a tiny random number of stars at random X positions
    UINT8 count = rand8() % 3; // 0..2 stars per row
    for (UINT8 i = 0; i < count; i++) {
        UINT8 sx = rand8() % 20;
        unsigned char t = (rand8() & 1) ? 1 : 2; // bright or dim
        set_bkg_tile_xy(sx, row & 31u, t);
    }
}

static void starfield_init(void) {
    set_bkg_data(0, 3, bg_tiles);
    // Initialize full 32 tile rows so vertical scroll can wrap
    for (UINT8 y = 0; y < 32; y++) {
        starfield_fill_row(y);
    }
    scroll_y = 0;
    SCY_REG = scroll_y;
}

static void starfield_update(void) {
    // Scroll upward (stars move up)
    scroll_y--;
    SCY_REG = scroll_y;
    // Every 8 pixels, create a new row at the bottom as it comes into view
    if ((scroll_y & 7u) == 0u) {
        UINT8 bottom_row = ((scroll_y >> 3) + 18u) & 31u;
        starfield_fill_row(bottom_row);
    }
}

void main(void) {
    disable_interrupts();
    DISPLAY_OFF;

    // Sprites setup
    SHOW_SPRITES;
    SPRITES_8x16;
    init_sprite_palettes();
    init_palettes();
    load_sprite_assets();

    // Background starfield
    starfield_init();
    SHOW_BKG;
    DISPLAY_ON;
    enable_interrupts();

    entities_init();

    while(1) {
        wait_vbl_done();
        starfield_update();
        player_update();
        bullets_update();
        enemies_update();
        try_spawn_enemy();
        handle_collisions();
    }
}


