#include <gb/gb.h>
#include <rand.h>
#include <gb/cgb.h>
#include "graphics.h"
#include "entities.h"

Player player;
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];

static UINT8 rng_seed = 0;
static UINT8 enemy_spawn_cooldown = 0; // frames until next enemy can spawn
static UINT8 last_spawn_x = 80;
static UINT8 enemy_move_divider = 0;   // toggles to slow enemy motion

// In 8x16 mode one OAM entry displays two tiles vertically.
static void set_sprite_8x16(UINT8 oam_idx, UINT8 tile_base_even, UINT8 x, UINT8 y, UINT8 prop) {
    // Ensure even tile index
    set_sprite_tile(oam_idx, tile_base_even & 0xFE);
    set_sprite_prop(oam_idx, prop);
    move_sprite(oam_idx, x, y);
}

void entities_init(void) {
    // Player at bottom center
    player.x = 80;
    player.y = 120;
    player.sprite_idx = 0; // uses one OAM entry (8x16)
    player.can_shoot_cooldown = 0;

    // Player uses single 8x16
    SPRITES_8x16;
    set_sprite_8x16(player.sprite_idx, TILE_PLAYER_BASE + 0, player.x, player.y - 8, S_PAL(0));

    // Hide and reset bullets and enemies
    for (UINT8 i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
        bullets[i].sprite_idx = 1 + i; // pack tightly after player
        move_sprite(bullets[i].sprite_idx, 0, 0);
    }
    for (UINT8 i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
        enemies[i].sprite_idx = 1 + MAX_BULLETS + i;
        move_sprite(enemies[i].sprite_idx, 0, 0);
    }
}

void spawn_player_bullet(UINT8 x, UINT8 y) {
    for (UINT8 i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].active = 1;
            bullets[i].x = x;
            bullets[i].y = y;
            bullets[i].vx = 0;
            bullets[i].vy = -3;
            set_sprite_8x16(bullets[i].sprite_idx, TILE_BULLET_BASE + 0, bullets[i].x, bullets[i].y - 8, S_PAL(1));
            return;
        }
    }
}

void try_spawn_enemy(void) {
    // Enforce a cooldown so spawns aren't bunched into pairs
    if (enemy_spawn_cooldown > 0) {
        enemy_spawn_cooldown--;
        return;
    }

    // Simple RNG using frame counter
    rng_seed += 1;
    if ((rng_seed & 0x07) != 0) return; // ~1/8 chance when off cooldown

    for (UINT8 i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = 1;
            // pick an X not too close to previous spawn to avoid visual grouping
            UINT8 attempts = 0;
            UINT8 sx;
            do {
                sx = 16 + (rng_seed * 11u) % 144; // 16..159-?
                attempts++;
                rng_seed += 17; // advance rng
            } while (attempts < 5 && (sx > last_spawn_x ? (sx - last_spawn_x) < 16 : (last_spawn_x - sx) < 16));
            enemies[i].x = sx;
            last_spawn_x = sx;
            enemies[i].y = 16;
            enemies[i].vx = ((rng_seed & 1) ? 1 : -1);
            enemies[i].vy = 1;
            enemies[i].jitter_counter = rng_seed;
            set_sprite_8x16(enemies[i].sprite_idx, TILE_ENEMY_BASE + 0, enemies[i].x, enemies[i].y - 8, S_PALETTE);
            // Set cooldown to space out spawns (avoid groups of two)
            enemy_spawn_cooldown = 20; // ~1/3 second at 60fps
            return;
        }
    }
}

void player_update(void) {
    UINT8 keys = joypad();
    if (keys & J_LEFT)  { if (player.x > 8) player.x -= 2; }
    if (keys & J_RIGHT) { if (player.x < 160) player.x += 2; }
    if (keys & J_UP)    { if (player.y > 24) player.y -= 2; }
    if (keys & J_DOWN)  { if (player.y < 144) player.y += 2; }

    // Shooting cooldown
    if (player.can_shoot_cooldown > 0) player.can_shoot_cooldown--;
    if ((keys & J_A) && (player.can_shoot_cooldown == 0)) {
        spawn_player_bullet(player.x, player.y - 16);
        player.can_shoot_cooldown = 8;
    }

    // Update player OAM
    set_sprite_8x16(player.sprite_idx, TILE_PLAYER_BASE + 0, player.x, player.y - 8, 0);
}

void bullets_update(void) {
    for (UINT8 i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        bullets[i].x += bullets[i].vx;
        bullets[i].y += bullets[i].vy;
        if (bullets[i].y < 16) {
            bullets[i].active = 0;
            move_sprite(bullets[i].sprite_idx, 0, 0);
            continue;
        }
        set_sprite_8x16(bullets[i].sprite_idx, TILE_BULLET_BASE + 0, bullets[i].x, bullets[i].y - 8, S_PAL(1));
    }
}

void enemies_update(void) {
    // Toggle divider to move enemies every other frame
    enemy_move_divider ^= 1;
    for (UINT8 i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        // Jitter: less frequent and milder to reduce flickery motion
        enemies[i].jitter_counter++;
        if ((enemies[i].jitter_counter & 0x0Fu) == 0u) { // ~ every 16 frames
            UINT8 r = (rng_seed += 13);
            // 1/4 chance to flip horizontal direction, otherwise keep current
            if ((r & 0x03u) == 0u) {
                enemies[i].vx = (enemies[i].vx == 0) ? ((r & 0x10) ? 1 : -1) : (INT8)(-enemies[i].vx);
            }
            // Vertical: mostly keep moving down; rare short pause
            enemies[i].vy = ((r & 0x30u) == 0x30u) ? 0 : 1;
        }
        if (!enemy_move_divider) {
            enemies[i].x += enemies[i].vx;
            enemies[i].y += enemies[i].vy;
        }

        // Despawn if they pass the bottom; otherwise allow going out of bounds
        if (enemies[i].y > 168) { // ensure fully off-screen
            enemies[i].active = 0;
            move_sprite(enemies[i].sprite_idx, 0, 0);
            continue;
        }
        // Bounce horizontally at screen edges
        if (enemies[i].x <= 8) {
            enemies[i].x = 8;
            if (enemies[i].vx < 0) enemies[i].vx = 1;
        } else if (enemies[i].x >= 160) {
            enemies[i].x = 160;
            if (enemies[i].vx > 0) enemies[i].vx = -1;
        }
            set_sprite_8x16(enemies[i].sprite_idx, TILE_ENEMY_BASE + 0, enemies[i].x, enemies[i].y - 8, S_PAL(2));
        set_sprite_8x16(enemies[i].sprite_idx, TILE_ENEMY_BASE + 0, enemies[i].x, enemies[i].y - 8, S_PAL(2));
    }
}

static UINT8 aabb_overlap(UINT8 x1, UINT8 y1, UINT8 w1, UINT8 h1, UINT8 x2, UINT8 y2, UINT8 w2, UINT8 h2) {
    return !(x2 > x1 + w1 || x2 + w2 < x1 || y2 > y1 + h1 || y2 + h2 < y1);
}

void handle_collisions(void) {
    for (UINT8 e = 0; e < MAX_ENEMIES; e++) {
        if (!enemies[e].active) continue;
        for (UINT8 b = 0; b < MAX_BULLETS; b++) {
            if (!bullets[b].active) continue;
            if (aabb_overlap(bullets[b].x - 4, bullets[b].y - 8, 4, 8, enemies[e].x - 8, enemies[e].y - 8, 16, 16)) {
                enemies[e].active = 0;
                bullets[b].active = 0;
                move_sprite(enemies[e].sprite_idx, 0, 0);
                move_sprite(bullets[b].sprite_idx, 0, 0);
                break;
            }
        }
    }
}


