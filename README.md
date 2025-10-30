# gbc-shooter (GBDK-2020 scaffold)

Minimal starter using [GBDK-2020](https://github.com/gbdk-2020/gbdk-2020) that displays a white screen with a green rectangle for testing.

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

## Notes
- Works on DMG and CGB. On CGB, the rectangle is true green; on DMG, greens are shades due to hardware palette.
- Modify rectangle tile area in `src/main.c` via `x, y, w, h`.
