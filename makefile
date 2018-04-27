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
SFX_CONVERTER=tools/neslib_famitracker/tools/nsf2data
AFTER_SFX_CONVERTER=mv sound/sfx/sfx.s sound/sfx/generated/sfx.s

# Built-in tools: 
CHR2IMG=tools/chr2img/chr2img
TMX2C=tools/tmx2c/tmx2c
SPRITE_DEF2IMG=tools/sprite_def2img/sprite_def2img

# Javascript versions of built-in tools: (Uncomment these if you're working on the tools)
# CHR2IMG=node tools/chr2img/src/index.js
# TMX2C=node tools/tmx2c/src/index.js
# SPRITE_DEF2IMG=node tools/sprite_def2img/src/index.js

SOUND_BANK=0

SOURCE_LEVELS_TMX=$(strip $(call rwildcard, levels/, *.tmx))
SOURCE_LEVELS_C=$(subst levels/, temp/level_, $(patsubst %.tmx, %.c, $(SOURCE_LEVELS_TMX)))

SOURCE_C=$(SOURCE_LEVELS_C) $(strip $(call rwildcard, source/, *.c))
SOURCE_S=$(patsubst source/, temp/, $(patsubst %.c, %.s, $(SOURCE_C)))
SOURCE_O=$(addprefix temp/, $(notdir $(patsubst %.s, %.o, $(SOURCE_S))))
SOURCE_DIRS=$(sort $(dir $(call rwildcard, source, %))) temp
SOURCE_CRT0_ASM=$(strip $(call rwildcard, source/, *.asm))
SOURCE_CRT0_GRAPHICS=$(strip $(call rwildcard, graphics/, *.pal)) $(strip $(call rwildcard, graphics/, *.chr))
SOURCE_HEADERS=$(strip $(call rwildcard, source/, *.h))

VPATH=$(SOURCE_DIRS)
# Uses the windows command line to open your rom, 
# which effectively does the same thing as double-clicking the rom in explorer.
MAIN_EMULATOR=cmd /c start

CONFIG_FILE=tools/cc65_config/game.cfg

# Path to 7-Zip - only used for generating tools zip. There's a 99.9% chance you don't care about this.
7ZIP="/cygdrive/c/Program Files/7-Zip/7z"
# ===== ENGINE SETTINGS END HERE =====

# ===== Actual makefile logic starts here =====
# You really shouldn't need to edit anything below this line if you're not doing advanced stuff.

# Disable generating sfx and music on circleci
ifdef CIRCLECI
	SFX_CONVERTER=echo -q
	AFTER_SFX_CONVERTER=echo Skipping SFX Generation...
endif

# Cancelling a couple implicit rules that confuse us greatly
%.o : %.s
%.o : %.c
%.s : %.c

# Keeping all intermediate files, so debugging and learning is easier. :)
# There are probably better ways to do this, and I know things are still deleted. Feel free to PR a better way in.
.PRECIOUS: $(SOURCE_C) $(SOURCE_S), $(SOURCE_O)

build: rom/$(ROM_NAME).nes graphics/generated/tiles.png graphics/generated/sprites.png

build-tiles: graphics/generated/tiles.png
build-sprites: graphics/generated/sprites.png

temp/crt0.o: source/neslib_asm/crt0.asm $(SOURCE_CRT0_ASM) $(SOURCE_CRT0_GRAPHICS) sound/music/music.bin sound/music/samples.bin sound/sfx/generated/sfx.s
	$(MAIN_ASM_COMPILER) source/neslib_asm/crt0.asm -o temp/crt0.o -D SOUND_BANK=$(SOUND_BANK)

# This bit is a little cheap... any time a header file changes, just recompile all C files. There might
# be some trickery we could do to find all C files that actually care, but this compiles fast enough that 
# it shouldn't be a huge deal.
temp/%.s: %.c $(SOURCE_HEADERS)
	$(MAIN_COMPILER) -Oi $< --add-source --include-dir ./tools/cc65/include -o $(patsubst %.o, %.s, $@)

temp/%.o: temp/%.s
	$(MAIN_ASM_COMPILER) $< 

temp/%.s: temp/%.c
	$(MAIN_COMPILER) -Oi $< --add-source --include-dir ./tools/cc65/include -o $(patsubst %.o, %.s, $@)

temp/level_%.c: levels/%.tmx
	$(TMX2C) 3 overworld $< $(patsubst %.c, %, $@)

graphics/generated/tiles.png: graphics/tiles.chr graphics/sprites.chr graphics/palettes/main_bg.pal
	$(CHR2IMG) graphics/tiles.chr graphics/palettes/main_bg.pal graphics/generated/tiles.png

graphics/generated/sprites.png: graphics/tiles.chr graphics/sprites.chr graphics/palettes/main_sprite.pal source/sprites/sprite_definitions.c
	$(SPRITE_DEF2IMG) ./source/sprites/sprite_definitions.c ./graphics/sprites.chr ./graphics/palettes/main_sprite.pal graphics/generated/sprites.png

sound/sfx/generated/sfx.s: sound/sfx/sfx.nsf
	$(SFX_CONVERTER) sound/sfx/sfx.nsf -ca65 -ntsc && sleep 1 && $(AFTER_SFX_CONVERTER)

rom/$(ROM_NAME).nes: temp/crt0.o $(SOURCE_O)
	$(MAIN_LINKER) -C $(CONFIG_FILE) -o rom/$(ROM_NAME).nes temp/*.o tools/neslib_famitracker/runtime.lib

# Build up the tool zip that's saved on the website/etc. There's a 99.9% chance you don't care about this.
# Meant to be run from the base folder of nes-starter-kit - all node stuff must be compiled!
build_tool_zip: 
	-rm -f temp/tools.zip
	$(7ZIP) a temp/tools.zip tools/cc65 tools/chr2img/chr2img.exe tools/chr2img/LICENSE tools/nessc tools/nesst tools/tmx2c/tmx2c.exe tools/sprite_def2img/sprite_def2img.exe tools/sprite_Def2img/LICENSE tools/tmx2c/LICENSE tools/neslib_famitracker tools/misc tools/install_cygwin.bat ./tools/zip_readme/readme.txt



clean:
	-rm -f rom/*.nes
	-rm -rf temp/levels
	-rm -f temp/*
	-rm -f sounds/sfx/generated/*.s
	-rm -f graphics/generated/*.png
	mkdir temp/levels
	touch temp/empty
	touch temp/levels/empty

run:
	$(MAIN_EMULATOR) rom/$(ROM_NAME).nes

space_check:
	$(SPACE_CHECKER) rom/$(ROM_NAME).nes
