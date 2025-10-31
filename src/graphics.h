#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <gb/gb.h>
#include <gb/cgb.h>

// Sprite indices in VRAM
#define TILE_PLAYER_BASE 0
#define TILE_ENEMY_BASE 8
#define TILE_BULLET_BASE 12
#define TILE_UI_BASE 14

// Number of tiles per sprite kind
#define PLAYER_TILES 8    // 8 tiles for 8x16 (two 8x8 per sprite, 4 sprites tall)
#define ENEMY_TILES 4
#define BULLET_TILES 2
#define UI_TILES 2

extern const unsigned char player_tiles[];
extern const unsigned char enemy_tiles[];
extern const unsigned char bullet_tiles[];
extern const unsigned char hud_tiles[];

void load_sprite_assets(void);
void init_sprite_palettes(void);

#endif

