#include <gb/gb.h>
#include <gb/cgb.h>
#include "graphics.h"
#include "entities.h"
#include "starfield.h"

static void init_palettes(void) { starfield_set_palettes(); }

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


