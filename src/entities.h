#ifndef ENTITIES_H
#define ENTITIES_H

#include <gb/gb.h>

typedef struct {
    UINT8 x;
    UINT8 y;
    INT8  vx;
    INT8  vy;
    UINT8 active;
    UINT8 sprite_idx; // starting OAM index (in 8x16 mode consumes 2 indices per visual sprite)
} Bullet;

typedef struct {
    UINT8 x;
    UINT8 y;
    INT8  vx;
    INT8  vy;
    UINT8 active;
    UINT8 sprite_idx; // uses 2 OAM entries (8x16)
} Enemy;

typedef struct {
    UINT8 x;
    UINT8 y;
    UINT8 sprite_idx; // uses 4 OAM entries (stacked 8x16s)
    UINT8 can_shoot_cooldown;
} Player;

#define MAX_BULLETS 8
#define MAX_ENEMIES 6

extern Player player;
extern Bullet bullets[MAX_BULLETS];
extern Enemy enemies[MAX_ENEMIES];

void entities_init(void);
void player_update(void);
void bullets_update(void);
void enemies_update(void);
void spawn_player_bullet(UINT8 x, UINT8 y);
void try_spawn_enemy(void);
void handle_collisions(void);

#endif

