# Minimal GBDK-2020 project Makefile
# Requires GBDK-2020 installed and GBDK_HOME pointing to its root (e.g., /opt/gbdk)

GBDK_HOME ?= /opt/gbdk
LCC := $(GBDK_HOME)/bin/lcc

# Output
PROJECT := gbc-shooter
BUILD_DIR := build
TARGET := $(BUILD_DIR)/$(PROJECT)

# Sources
SRC_DIR := src
CSRC := $(SRC_DIR)/main.c

# Flags
# -Wa-l: generate assembler listing; -Wl-m: map file; -Wl-j: auto-join banks; -O2: optimize
CFLAGS := -Wa-l -Wl-m -Wl-j -O2

ROM := $(TARGET).gb

.PHONY: all clean dirs

all: dirs $(ROM)

dirs:
	mkdir -p $(BUILD_DIR)

$(ROM): $(CSRC)
	$(LCC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(BUILD_DIR)


