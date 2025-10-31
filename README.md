# gbc-shooter

Vertical shooter prototype built with [GBDK-2020](https://github.com/gbdk-2020/gbdk-2020).

Features:
- Player ship (8x16) can move freely and fire shots
- Enemies spawn with erratic motion and despawn off-screen
- Upward-scrolling starfield background
- Three-life system with heart HUD and game over screen
- Game Boy Color palettes (player green, bullets red, enemies yellow)
- Simple shoot/explosion sound effects
 - Title screen with logo and blinking "PRESS START" prompt
 - Start button transitions: Title → Play, Game Over → Title/Restart

## Prerequisites
- Install GBDK-2020 (download a release and extract)
- Set `GBDK_HOME` to the install path, e.g.:

```bash
export GBDK_HOME=/opt/gbdk
```

## Build

```bash
make
```

Output ROM: `build/gbc-shooter.gb`

## Run (emulator examples)

```bash
# SameBoy
sameboy build/gbc-shooter.gb

# mGBA
mgba-qt build/gbc-shooter.gb
```

Audio is generated with the Game Boy sound hardware; ensure your emulator audio is enabled to hear the effects.

## Controls
- D-Pad: Move ship
- A: Shoot
- Start: From Title → Start game; From Game Over → Return to Title

## Notes
- The ROM boots in Game Boy Color mode; on original DMG, colors appear as grayscale.
- Enemies bounce at screen edges, move with slight randomness, and despawn after leaving the bottom.
- The title screen is static (no scrolling); gameplay uses a scrolling starfield.
