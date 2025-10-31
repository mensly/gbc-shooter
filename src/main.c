#include <gb/gb.h>
#include <gb/cgb.h>
#include "graphics.h"
#include "entities.h"
#include "starfield.h"
#include "sound.h"

static void init_palettes(void) { starfield_set_palettes(); }

typedef enum {
    GAME_STATE_PLAY = 0,
    GAME_STATE_OVER
} GameState;

static GameState game_state = GAME_STATE_PLAY;

static void show_game_over_screen(void);

#define GAME_OVER_TILE_BASE 3
#define GAME_OVER_TILE_COUNT 7

enum {
    TILE_G = GAME_OVER_TILE_BASE + 0,
    TILE_A,
    TILE_M,
    TILE_E,
    TILE_O,
    TILE_V,
    TILE_R
};

static const unsigned char game_over_tiles[] = {
    // G
    0x3C,0x00,0x60,0x00,0x60,0x00,0x6E,0x00,
    0x66,0x00,0x66,0x00,0x3E,0x00,0x00,0x00,
    // A
    0x18,0x00,0x24,0x00,0x42,0x00,0x7E,0x00,
    0x42,0x00,0x42,0x00,0x42,0x00,0x00,0x00,
    // M
    0x42,0x00,0x66,0x00,0x7E,0x00,0x5A,0x00,
    0x42,0x00,0x42,0x00,0x42,0x00,0x00,0x00,
    // E
    0x7E,0x00,0x40,0x00,0x40,0x00,0x7C,0x00,
    0x40,0x00,0x40,0x00,0x7E,0x00,0x00,0x00,
    // O
    0x3C,0x00,0x42,0x00,0x42,0x00,0x42,0x00,
    0x42,0x00,0x42,0x00,0x3C,0x00,0x00,0x00,
    // V
    0x42,0x00,0x42,0x00,0x42,0x00,0x24,0x00,
    0x24,0x00,0x18,0x00,0x18,0x00,0x00,0x00,
    // R
    0x7C,0x00,0x42,0x00,0x42,0x00,0x7C,0x00,
    0x48,0x00,0x44,0x00,0x42,0x00,0x00,0x00,
};

static const unsigned char game_over_row1[] = { TILE_G, TILE_A, TILE_M, TILE_E };
static const unsigned char game_over_row2[] = { TILE_O, TILE_V, TILE_E, TILE_R };

// starfield moved to starfield.c

void main(void) {
    disable_interrupts();
    DISPLAY_OFF;

    // Sprites setup
    SHOW_SPRITES;
    SPRITES_8x16;
    init_sprite_palettes();
    init_palettes();
    load_sprite_assets();
    sound_init();

    // Background starfield
    starfield_init();
    SHOW_BKG;
    DISPLAY_ON;
    enable_interrupts();

    entities_init();
    game_state = GAME_STATE_PLAY;

    while(1) {
        wait_vbl_done();
        if (game_state == GAME_STATE_PLAY) {
            starfield_update();
            player_update();
            bullets_update();
            enemies_update();
            try_spawn_enemy();
            handle_collisions();
            if (entities_is_game_over()) {
                game_state = GAME_STATE_OVER;
                show_game_over_screen();
            }
        }
    }
}

static void show_game_over_screen(void) {
    DISPLAY_OFF;
    HIDE_SPRITES;
    set_bkg_data(GAME_OVER_TILE_BASE, GAME_OVER_TILE_COUNT, game_over_tiles);
    for (UINT8 y = 0; y < 18; y++) {
        for (UINT8 x = 0; x < 20; x++) {
            set_bkg_tile_xy(x, y, 0);
        }
    }
    SCY_REG = 0;
    set_bkg_tiles(7, 8, 4, 1, game_over_row1);
    set_bkg_tiles(7, 9, 4, 1, game_over_row2);
    SHOW_BKG;
    DISPLAY_ON;
}


