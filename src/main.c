#include <gb/gb.h>
#include <gb/cgb.h>
#include "graphics.h"
#include "entities.h"
#include "starfield.h"
#include "sound.h"

static void init_palettes(void) { starfield_set_palettes(); }

typedef enum {
    GAME_STATE_TITLE = 0,
    GAME_STATE_PLAY,
    GAME_STATE_OVER
} GameState;

static GameState game_state = GAME_STATE_PLAY;

static void show_game_over_screen(void);
static void show_title_screen(void);

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

static const unsigned char game_text_tiles[] = {
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
    // S
    0x3E,0x00,0x40,0x00,0x40,0x00,0x3C,0x00,
    0x02,0x00,0x02,0x00,0x7C,0x00,0x00,0x00,
    // P
    0x7C,0x00,0x42,0x00,0x42,0x00,0x7C,0x00,
    0x40,0x00,0x40,0x00,0x40,0x00,0x00,0x00,
    // C
    0x3C,0x00,0x42,0x00,0x40,0x00,0x40,0x00,
    0x40,0x00,0x42,0x00,0x3C,0x00,0x00,0x00,
    // H
    0x42,0x00,0x42,0x00,0x42,0x00,0x7E,0x00,
    0x42,0x00,0x42,0x00,0x42,0x00,0x00,0x00,
    // T
    0x7E,0x00,0x18,0x00,0x18,0x00,0x18,0x00,
    0x18,0x00,0x18,0x00,0x18,0x00,0x00,0x00,
    // R (duplicate of R above, included for indexing continuity)
    0x7C,0x00,0x42,0x00,0x42,0x00,0x7C,0x00,
    0x48,0x00,0x44,0x00,0x42,0x00,0x00,0x00,
};

static const unsigned char game_over_row1[] = { TILE_G, TILE_A, TILE_M, TILE_E };
static const unsigned char game_over_row2[] = { TILE_O, TILE_V, TILE_E, TILE_R };
// Title: "SPACE SHOOTER" and "PRESS START"
// Additional indices after R: S,P,C,H,T,R
#define TILE_S (GAME_OVER_TILE_BASE + 7)
#define TILE_P (GAME_OVER_TILE_BASE + 8)
#define TILE_C (GAME_OVER_TILE_BASE + 9)
#define TILE_H (GAME_OVER_TILE_BASE + 10)
#define TILE_T (GAME_OVER_TILE_BASE + 11)
#define TILE_R2 (GAME_OVER_TILE_BASE + 12)
static const unsigned char title_row1[] = { TILE_S, TILE_P, TILE_A, TILE_C, TILE_E, 0, TILE_S, TILE_H, TILE_O, TILE_O, TILE_T, TILE_E, TILE_R2 };
static const unsigned char title_prompt[] = { TILE_P, TILE_R, TILE_E, TILE_S, TILE_S, 0, TILE_S, TILE_T, TILE_A, TILE_R, TILE_T };

// Simple 16x16 logo (2x2 tiles) 
#define TILE_LOGO (GAME_OVER_TILE_BASE + 13)
static const unsigned char logo_tiles[] = {
    // Top-left
    0x00,0x00,0x18,0x00,0x3C,0x00,0x7E,0x00,
    0x7E,0x00,0x3C,0x00,0x18,0x00,0x00,0x00,
    // Top-right
    0x00,0x00,0x18,0x00,0x3C,0x00,0x7E,0x00,
    0x7E,0x00,0x3C,0x00,0x18,0x00,0x00,0x00,
    // Bottom-left
    0x00,0x00,0x18,0x00,0x3C,0x00,0x7E,0x00,
    0x7E,0x00,0x3C,0x00,0x18,0x00,0x00,0x00,
    // Bottom-right
    0x00,0x00,0x18,0x00,0x3C,0x00,0x7E,0x00,
    0x7E,0x00,0x3C,0x00,0x18,0x00,0x00,0x00,
};

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

    // Title screen
    starfield_init();
    SHOW_BKG;
    DISPLAY_ON;
    enable_interrupts();
    show_title_screen();
    game_state = GAME_STATE_TITLE;

    while(1) {
        wait_vbl_done();
        UINT8 keys = joypad();
        static UINT8 prev_keys = 0;
        if (game_state == GAME_STATE_TITLE) {
            // Keep title screen static (no scroll)
            SCY_REG = 0;
            static UINT8 blink = 0; blink++;
            if ((blink & 0x10) == 0) {
                set_bkg_tiles(4, 12, sizeof(title_prompt), 1, title_prompt);
            } else {
                // hide prompt by writing spaces (tile 0)
                for (UINT8 i = 0; i < sizeof(title_prompt); i++) set_bkg_tile_xy(4 + i, 12, 0);
            }
            if ((keys & J_START) && !(prev_keys & J_START)) {
                // Reset background to starfield and show sprites for gameplay
                starfield_init();
                entities_init();
                SHOW_SPRITES;
                game_state = GAME_STATE_PLAY;
            }
        } else if (game_state == GAME_STATE_PLAY) {
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
        } else if (game_state == GAME_STATE_OVER) {
            // Wait for Start to restart back to title
            if ((keys & J_START) && !(prev_keys & J_START)) {
                // Reset background to starfield and show sprites for gameplay
                HIDE_SPRITES;
                starfield_init();
                entities_init();
                player_update();
                enemies_update();
                SHOW_SPRITES;
                game_state = GAME_STATE_PLAY;
            }
        }
        prev_keys = keys;
    }
}

static void show_game_over_screen(void) {
    DISPLAY_OFF;
    HIDE_SPRITES;
    set_bkg_data(GAME_OVER_TILE_BASE, GAME_OVER_TILE_COUNT + 6, game_text_tiles);
    for (UINT8 y = 0; y < 18; y++) {
        for (UINT8 x = 0; x < 20; x++) {
            set_bkg_tile_xy(x, y, 0);
        }
    }
    SCY_REG = 0;
    set_bkg_tiles(7, 8, 4, 1, game_over_row1);
    set_bkg_tiles(7, 9, 4, 1, game_over_row2);
    // Prompt
    set_bkg_tiles(4, 12, sizeof(title_prompt), 1, title_prompt);
    SHOW_BKG;
    DISPLAY_ON;
}

static void show_title_screen(void) {
    DISPLAY_OFF;
    HIDE_SPRITES;
    set_bkg_data(GAME_OVER_TILE_BASE, GAME_OVER_TILE_COUNT + 6, game_text_tiles);
    set_bkg_data(TILE_LOGO, 4, logo_tiles);
    for (UINT8 y = 0; y < 18; y++) {
        for (UINT8 x = 0; x < 20; x++) {
            set_bkg_tile_xy(x, y, 0);
        }
    }
    SCY_REG = 0;
    // Title centered (13 chars wide), start at x=3
    set_bkg_tiles(3, 7, sizeof(title_row1), 1, title_row1);
    // Logo 2x2 tiles centered above title (x=9,y=4)
    unsigned char logo_map[4] = { TILE_LOGO+0, TILE_LOGO+1, TILE_LOGO+2, TILE_LOGO+3 };
    set_bkg_tiles(9, 4, 2, 2, logo_map);
    set_bkg_tiles(4, 12, sizeof(title_prompt), 1, title_prompt);
    SHOW_BKG;
    DISPLAY_ON;
}


