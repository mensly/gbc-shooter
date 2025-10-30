#include <gb/gb.h>
#include <gb/cgb.h>

// Two tiles: index 0 = blank (all color 0), index 1 = solid (all color 1)
// Each tile is 16 bytes (2bpp, 8 rows * 2 bytes per row)
static const unsigned char tiles_2bpp[] = {
    // Tile 0: blank (all pixels color index 0)
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    // Tile 1: solid color index 1 (lo-plane=1s, hi-plane=0s)
    0xFF,0x00, 0xFF,0x00, 0xFF,0x00, 0xFF,0x00,
    0xFF,0x00, 0xFF,0x00, 0xFF,0x00, 0xFF,0x00
};

void init_palettes(void) {
    // DMG (original Game Boy): set BGP so color 0 = white, 1 = dark (greenish on typical palettes)
    BGP_REG = 0b11100100; // 00->white, 01->light gray, 10->dark gray, 11->black (default)

    // CGB: set background palette 0: [0]=white, [1]=green, others unused
    if (_cpu == CGB_TYPE) {
        const palette_color_t cgb_palette[] = {
            RGB(31,31,31), // white
            RGB(0,31,0),   // green
            RGB(0,0,0),    // unused
            RGB(0,0,0)     // unused
        };
        set_bkg_palette(0, 1, cgb_palette);
    }
}

void main(void) {
    disable_interrupts();
    DISPLAY_OFF;

    init_palettes();

    // Load tile data: 2 tiles starting at index 0
    set_bkg_data(0, 2, tiles_2bpp);

    // Clear background to blank tile (0) to get a white screen
    fill_bkg_rect(0, 0, 20, 18, 0);

    // Draw a green rectangle area using tile index 1
    // Adjust position/size as desired (tile coordinates: 0..19 x 0..17)
    const uint8_t x = 5;
    const uint8_t y = 5;
    const uint8_t w = 10; // tiles wide
    const uint8_t h = 6;  // tiles high
    fill_bkg_rect(x, y, w, h, 1);

    SHOW_BKG;
    DISPLAY_ON;
    enable_interrupts();

    // Idle loop
    while(1) {
        wait_vbl_done();
    }
}


