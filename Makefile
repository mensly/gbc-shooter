# Minimal GBDK-2020 project Makefile
# Requires GBDK-2020 installed and GBDK_HOME pointing to its root (e.g., /opt/gbdk)

LCC := $(GBDK_HOME)/bin/lcc

# Output
PROJECT := gbc-shooter
BUILD_DIR := build
TARGET := $(BUILD_DIR)/$(PROJECT)

# Sources
SRC_DIR := src
CSRC := $(wildcard $(SRC_DIR)/*.c)
HFILES := $(wildcard $(SRC_DIR)/*.h)

# Objects
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CSRC))

# Flags
# -Wa-l: generate assembler listing; -Wl-m: map file; -Wl-j: auto-join banks; -O2: optimize
# -Wm-yC: mark ROM as CGB-only (boot in Game Boy Color mode)
# -Wm-yt3: Cartridge type = MBC1 + RAM + BATTERY (enables .sav persistence)
# -Wm-ya1: RAM banks = 1 (8KB)
CFLAGS := -Wa-l -Wl-m -Wl-j -O2 -Wm-yC -Wm-yt3 -Wm-ya1

ROM := $(TARGET).gb

.PHONY: all clean dirs test

all: dirs $(ROM)

dirs:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(LCC) $(CFLAGS) -c -o $@ $<

$(ROM): $(OBJS) $(HFILES) Makefile
	$(LCC) $(CFLAGS) -o $@ $(OBJS)

test: all
	mgba-qt $(ROM)

clean:
	rm -rf $(BUILD_DIR)


