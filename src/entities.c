#include <gb/gb.h>
#include <rand.h>
#include "graphics.h"
#include "entities.h"

Player player;
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];

static UINT8 rng_seed = 0;

static void set_sprite_pair(UINT8 oam_idx, UINT8 tile_lo, UINT8 tile_hi, UINT8 x, UINT8 y, UINT8 prop) {
    set_sprite_tile(oam_idx, tile_lo);
    set_sprite_tile(oam_idx + 1, tile_hi);
    set_sprite_prop(oam_idx, prop);
    set_sprite_prop(oam_idx + 1, prop);
    move_sprite(oam_idx, x, y);
    move_sprite(oam_idx + 1, x, y + 8);
}

void entities_init(void) {
    // Player at bottom center
    player.x = 80;
    player.y = 120;
    player.sprite_idx = 0; // uses 0..3
    player.can_shoot_cooldown = 0;

    // Player uses 4 OAM entries (8x16 stacked)
    SPRITES_8x16;
    set_sprite_pair(player.sprite_idx, TILE_PLAYER_BASE + 0, TILE_PLAYER_BASE + 1, player.x, player.y - 24, 0); // top
    set_sprite_pair(player.sprite_idx + 2, TILE_PLAYER_BASE + 2, TILE_PLAYER_BASE + 3, player.x, player.y - 8, 0); // bottom

    // Hide and reset bullets and enemies
    for (UINT8 i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
        bullets[i].sprite_idx = 4 + (i * 2);
        move_sprite(bullets[i].sprite_idx, 0, 0);
        move_sprite(bullets[i].sprite_idx + 1, 0, 0);
    }
    for (UINT8 i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
        enemies[i].sprite_idx = 4 + (MAX_BULLETS * 2) + (i * 2);
        move_sprite(enemies[i].sprite_idx, 0, 0);
        move_sprite(enemies[i].sprite_idx + 1, 0, 0);
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
            set_sprite_pair(bullets[i].sprite_idx, TILE_BULLET_BASE + 0, TILE_BULLET_BASE + 1, bullets[i].x, bullets[i].y - 8, 0);
            return;
        }
    }
}

void try_spawn_enemy(void) {
    // Simple RNG using frame counter
    rng_seed += 1;
    if ((rng_seed & 0x0F) != 0) return; // roughly 1/16 chance per call

    for (UINT8 i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = 1;
            enemies[i].x = 16 + (rng_seed * 11u) % 144; // 16..159-?
            enemies[i].y = 16;
            enemies[i].vx = ((rng_seed & 1) ? 1 : -1);
            enemies[i].vy = 1;
            set_sprite_pair(enemies[i].sprite_idx, TILE_ENEMY_BASE + 0, TILE_ENEMY_BASE + 1, enemies[i].x, enemies[i].y - 8, S_PALETTE);
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
    set_sprite_pair(player.sprite_idx, TILE_PLAYER_BASE + 0, TILE_PLAYER_BASE + 1, player.x, player.y - 24, 0);
    set_sprite_pair(player.sprite_idx + 2, TILE_PLAYER_BASE + 2, TILE_PLAYER_BASE + 3, player.x, player.y - 8, 0);
}

void bullets_update(void) {
    for (UINT8 i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        bullets[i].x += bullets[i].vx;
        bullets[i].y += bullets[i].vy;
        if (bullets[i].y < 16) {
            bullets[i].active = 0;
            move_sprite(bullets[i].sprite_idx, 0, 0);
            move_sprite(bullets[i].sprite_idx + 1, 0, 0);
            continue;
        }
        set_sprite_pair(bullets[i].sprite_idx, TILE_BULLET_BASE + 0, TILE_BULLET_BASE + 1, bullets[i].x, bullets[i].y - 8, 0);
    }
}

void enemies_update(void) {
    for (UINT8 i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        enemies[i].x += enemies[i].vx;
        enemies[i].y += enemies[i].vy;

        if (enemies[i].x < 8 || enemies[i].x > 160) enemies[i].vx = -enemies[i].vx;
        if (enemies[i].y > 160) {
            enemies[i].active = 0;
            move_sprite(enemies[i].sprite_idx, 0, 0);
            move_sprite(enemies[i].sprite_idx + 1, 0, 0);
            continue;
        }
        set_sprite_pair(enemies[i].sprite_idx, TILE_ENEMY_BASE + 0, TILE_ENEMY_BASE + 1, enemies[i].x, enemies[i].y - 8, S_PALETTE);
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
                move_sprite(enemies[e].sprite_idx + 1, 0, 0);
                move_sprite(bullets[b].sprite_idx, 0, 0);
                move_sprite(bullets[b].sprite_idx + 1, 0, 0);
                break;
            }
        }
    }
}


