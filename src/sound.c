#include <gb/gb.h>
#include "sound.h"

void sound_init(void) {
    NR52_REG = 0x80; // Sound on
    NR50_REG = 0x77; // Max volume both speakers
    NR51_REG = 0xFF; // Enable all channels
}

void sound_play_shoot(void) {
    // Channel 1 square wave with quick sweep
    NR10_REG = 0x16; // sweep: shift 6, decrease, 1 sweep
    NR11_REG = 0x40; // duty 50%, length ignored
    NR12_REG = 0xF3; // envelope: vol=15, decrease, step length 3
    NR13_REG = 0x00; // frequency low
    NR14_REG = 0xC3; // trigger, freq high, no length counter
}

void sound_play_enemy_hit(void) {
    // Channel 4 noise burst
    NR41_REG = 0x10; // length (ignored)
    NR42_REG = 0xF1; // envelope: vol=15, decrease, step 1
    NR43_REG = 0x35; // poly counter: medium pitch noise
    NR44_REG = 0xC0; // restart sound
}


