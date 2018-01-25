# This is the main script that compiles everything for your game. 
# There are some settings up at the top you can change, but hopefully you will not have to touch this 
# otherwise.
#

# ===== USER SETTINGS START HERE =====

# The name of the output rom, without the trailing .nes.
ROM_NAME=starter

# ===== USER SETTINGS END HERE =====


# ===== ENGINE FUNCTIONS START HERE =====
# Hat-tip: https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make/18258352#18258352
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
# ===== ENGINE FUNCTIONS END HERE =====

# ===== ENGINE SETTINGS START HERE =====
# Note: You can edit these if you know what you're doing, but hopefully you don't have to. 
MAIN_COMPILER=./tools/cc65/bin/cc65
MAIN_ASM_COMPILER=./tools/cc65/bin/ca65
MAIN_LINKER=./tools/cc65/bin/ld65
MAP_PARSER=./tools/tmx2c/tmx2c 
SPACE_CHECKER=tools/nessc/nessc
SOUND_BANK=0

# FIXME: Unused, questionably useful.
SOURCE_LEVELS_TMX=$(strip $(call rwildcard, levels/, *.tmx))
SOURCE_LEVELS_C=$(patsubst levels/, temp/, $(patsubst %.tmx, %.c, $(SOURCE_LEVELS_TMX)))

SOURCE_C=$(strip $(call rwildcard, source/, *.c))
SOURCE_S=$(patsubst source/, temp/, $(patsubst %.c, %.s, $(SOURCE_C)))
SOURCE_O=$(addprefix temp/, $(notdir $(patsubst %.s, %.o, $(SOURCE_S))))
SOURCE_DIRS=$(sort $(dir $(call rwildcard, source, %)))
VPATH=$(SOURCE_DIRS)
# Uses the windows command line to open your rom, 
# which effectively does the same thing as double-clicking the rom in explorer.
MAIN_EMULATOR=cmd /c start

CONFIG_FILE=tools/cc65_config/game.cfg
# ===== ENGINE SETTINGS END HERE =====

# ===== Actual makefile logic starts here =====
# You really shouldn't need to edit anything below this line if you're not doing advanced stuff.

build: rom/$(ROM_NAME).nes

temp/crt0.o: source/neslib_asm/crt0.asm
	$(MAIN_ASM_COMPILER) source/neslib_asm/crt0.asm -o temp/crt0.o -D SOUND_BANK=$(SOUND_BANK)

temp/%.s: %.c
	$(MAIN_COMPILER) -Oi $< --add-source --include-dir ./tools/cc65/include -o $(patsubst %.o, %.s, $@)
temp/%.o: temp/%.s
	$(MAIN_ASM_COMPILER) $< 


rom/$(ROM_NAME).nes: temp/crt0.o $(SOURCE_O)
	$(MAIN_LINKER) -C $(CONFIG_FILE) -o rom/$(ROM_NAME).nes temp/*.o tools/neslib_famitracker/runtime.lib

clean:
	-rm -f rom/*.nes
	-rm -f temp/*
	touch temp/empty

run:
	$(MAIN_EMULATOR) rom/$(ROM_NAME).nes

space_check:
	$(SPACE_CHECKER) rom/$(ROM_NAME).nes
