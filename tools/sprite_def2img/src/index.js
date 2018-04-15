/**
 * This is a quick and dirty script used to parse output from tiled into map files. Unlike prior versions, this
 * also uses tmx files directly, and is compilable into a binary for various operating systems, so nodejs isn't 
 * required! 
 * 
 * That said, it's still very quick and dirty. I tried to document some of it, but this is likely the least polished
 * bit of the library. Don't be afraid to reach out for help if you're tweaking this. I hope it meets most needs
 * as-is.
 */
var VERSION = '1.0.0';

// Expects exactly the number of params we expect (first param is always node)
if (process.argv.length != 6) {
    printUsage();
    process.exit(1);
}

// Lookup table to convert NES colors to rgb.
// Stolen from: http://www.thealmightyguru.com/Games/Hacking/Wiki/index.php/NES_Palette
var nesRgbLookup = [
    124,124,124,
    0,0,252,
    0,0,188,
    68,40,188,
    148,0,132,
    168,0,32,
    168,16,0,
    136,20,0,
    80,48,0,
    0,120,0,
    0,104,0,
    0,88,0,
    0,64,88,
    0,0,0,
    0,0,0,
    0,0,0,
    188,188,188,
    0,120,248,
    0,88,248,
    104,68,252,
    216,0,204,
    228,0,88,
    248,56,0,
    228,92,16,
    172,124,0,
    0,184,0,
    0,168,0,
    0,168,68,
    0,136,136,
    0,0,0,
    0,0,0,
    0,0,0,
    248,248,248,
    60,188,252,
    104,136,252,
    152,120,248,
    248,120,248,
    248,88,152,
    248,120,88,
    252,160,68,
    248,184,0,
    184,248,24,
    88,216,84,
    88,248,152,
    0,232,216,
    120,120,120,
    0,0,0,
    0,0,0,
    252,252,252,
    164,228,252,
    184,184,248,
    216,184,248,
    248,184,248,
    248,164,192,
    240,208,176,
    252,224,168,
    248,216,120,
    216,248,120,
    184,248,184,
    184,248,216,
    0,252,252,
    248,216,248,
    0,0,0,
    0,0,0,
];


// Using the lookup table above, convert a color for the NES to its RGBA equivalent, so Jimp can understand.
function nesToRgb(id) {
    return Jimp.rgbaToInt(nesRgbLookup[id*3], nesRgbLookup[id*3+1], nesRgbLookup[id*3+2], 255);
}

// Use this to translate from a loop index to a single bit. 
var reverseBitmaskLookup = [
    0b10000000,
    0b01000000,
    0b00100000,
    0b00010000,
    0b00001000,
    0b00000100,
    0b00000010,
    0b00000001
];

var fs = require('fs'),
    logLevel = 'info', // change to 'verbose' for some extra output
    Jimp = require('jimp'),
    spriteDefFile = process.argv[2],
    spriteChrFile = process.argv[3],
    spritePalFile = process.argv[4],
    spriteOutFile = process.argv[5],
    TILE_ID_OFFSET = 1,
    PALETTE_OFFSET = 2,
    SCREEN_WIDTH = 16,
    SCREEN_HEIGHT = 12,
    SCREEN_HEIGHT_PADDED = 16,
    CHR_SPRITE_OFFSET = 0;

function printDate() {
    return '[' + new Date().toUTCString() + '] ';
}

function printUsage() {
    out('sprite_def2img version ' + VERSION);
    out('Usage: sprite_def2img [path/to/sprite_definitions.c] [path/to/main_sprite.chr] [path/to/main_sprite.pal] [path/to/output.png]');
}

function out() {
    var args = [].slice.call(arguments);
    args.unshift('[sprite_def2img] ', printDate());

    console.info.apply(this, args);
}

function verbose() {
    var args = [].slice.call(arguments);
    args.unshift('[sprite_def2img] ', printDate());

    if (logLevel == 'verbose') {
        console.info.apply(this, args);
    }
}


// Start simple; let's just read the C file in
var rawSpriteData = fs.readFileSync(spriteDefFile).toString();

// Now, strip all comments...
rawSpriteData = rawSpriteData.replace(/\/\*[\s\S]*?\*\/|([^\\:]|^)\/\/.*$/gm, '$1').trim();

// Now, find the var we care about...
var spriteDefinitionTextIndex = rawSpriteData.search(/spriteDefinitions\[\]\s=\s{/);
if (spriteDefinitionTextIndex === -1) {
    out('Failed finding sprite definitions in C file. Bailing out!');
    process.exit(1);
}
rawSpriteData = rawSpriteData.substr(spriteDefinitionTextIndex);
// Okay, now skip everything leading up to the object being opened
rawSpriteData = rawSpriteData.substr(rawSpriteData.indexOf('{') + 1);
rawSpriteData = rawSpriteData.substr(0, rawSpriteData.indexOf('}'));

// Okay, we should just have the var contents now... make it into a js array of strings...
var strSpriteData = rawSpriteData.split(',').map(function(str) { return str.trim(); });

if (strSpriteData.length % 8 != 0) {
    out('WARNING: Your sprite data is not a multiple of 8. This will likely break your sprite definitions...');
}
var spriteCount = strSpriteData.length / 8;
verbose('Found ' + spriteCount + ' sprites to draw...');
var parsedSpriteData = [];

// Now go through the sprites one-by-one, and figure out what to do with the values.
for (var i = 0; i < spriteCount; i++) {
    parsedSpriteData[i] = {baseIndex: i};
    parsedSpriteData[i].tileIdStr = strSpriteData[i*8 + TILE_ID_OFFSET];
    parsedSpriteData[i].sizePalStr = strSpriteData[i*8 + PALETTE_OFFSET];
    
    // Okay, we've got strings. Figure out a radix... is this base 10 (regular num) or base 16 (not)?
    var radix = 10;
    if (parsedSpriteData[i].tileIdStr.indexOf('0x') == 0){
        radix = 16;
    }
    parsedSpriteData[i].tileId = parseInt(parsedSpriteData[i].tileIdStr);
    parsedSpriteData[i].size = parsedSpriteData[i].sizePalStr.indexOf('8PX_8PX') !== -1 ? 8 : 16;
    parsedSpriteData[i].palette = 0;
    // Lame way to find the palette
    for (var j = 0; j < 4; j++) {
        if (parsedSpriteData[i].sizePalStr.indexOf('SPRITE_PALETTE_'+j) !== -1) {
            parsedSpriteData[i].palette = j;
        }
    }
}
verbose('Finished parsing.. starting to mess with images.');

// Okay, we actually have comprehendible sprite definitions that might even be accurate!
// Next step, load up the chr data, and start painting...

// Next up, let's load up the palette and chr...
var chrContents = fs.readFileSync(spriteChrFile),
    palContents = fs.readFileSync(spritePalFile);

    // List of all palettes based on NES color id. Each palette is an array within the big array.
    palettes = [
        [palContents[0], palContents[1], palContents[2], palContents[3]],
        [palContents[0], palContents[5], palContents[6], palContents[7]],
        [palContents[0], palContents[9], palContents[10], palContents[11]],
        [palContents[0], palContents[13], palContents[14], palContents[15]],
    ], 
    rgbPalettes = [];

// Loop over the list of NES palettes, and convert them to RGB palettes we can stick on an image.
palettes.forEach(function(pal, idx) {
    rgbPalettes[idx] = [nesToRgb(pal[0]), nesToRgb(pal[1]), nesToRgb(pal[2]), nesToRgb(pal[3])];
});

// Create a new image with the chr contents.
new Jimp(128, 128, function(err, image) {
    image.rgba(false);
    // See: http://wiki.nesdev.com/w/index.php/PPU_pattern_tables
    // Here's the bulk of what's going on... we loop over the chr file, and translate it to an image. 
    // Apologies, I'm not sure how to document this better...

    for (var tileRowId = 0; tileRowId < 16; tileRowId++) { // Loop over rows
        for (var tileId = 0; tileId < 16; tileId++) { // Loop over every tile in row 1
            for (var rowId = 0; rowId < 8; rowId++) { // Loop over every row in the tile
                for (var pixelId = 0; pixelId < 8; pixelId++) { // And every bit. (I'm sorry)
                    var colorId = (chrContents[CHR_SPRITE_OFFSET+(tileRowId * 256) + (tileId*16)+rowId] & reverseBitmaskLookup[pixelId] ? 1 : 0) +  (chrContents[CHR_SPRITE_OFFSET+(tileRowId * 256) + (tileId*16)+rowId+8] & reverseBitmaskLookup[pixelId] ? 2 : 0);

                    image.setPixelColor(colorId, tileId*8 + pixelId, tileRowId*8 + rowId);

                }
            }
        }
    }
    verbose('Base image created; writing individual tiles');


    // Okay, we made some image science. BUT, this is just the chr file. We need to move some stuff around.
    new Jimp(128, 128, function(err, spriteImage) {
        spriteImage.rgba(false);
        spriteImage.background(Jimp.rgbaToInt(0, 0, 0, 255));
        // Okay, we're going to compose this image of sprites until we run out. 
        // All are 16x16, and we want to tile left to right, top to bottom
        for (var i = 0; i < parsedSpriteData.length; i++) {

            var offset = parsedSpriteData[i].size == 8 ? 4 : 0,
                baseX = Math.floor(i%8) * 16 + offset,
                baseY = Math.floor(i/8) * 16 + offset;
            spriteImage.blit(image, baseX, baseY, Math.floor(parsedSpriteData[i].tileId%16) * 8, Math.floor(parsedSpriteData[i].tileId/16)*8, parsedSpriteData[i].size, parsedSpriteData[i].size);
            // Okay, we have the right image drawn... BUT, we need to re-color it :(
            // For every pixel in the image...
            for (var x = 0; x < parsedSpriteData[i].size; x++) {
                for (var y = 0; y < parsedSpriteData[i].size; y++) {
                    // Okay, get the color at this x/y... we just used 0-3 before, which conveniently is our index off rgbPalettes!
                    var color = spriteImage.getPixelColor(baseX + x, baseY +y);
                    color = rgbPalettes[parsedSpriteData[i].palette][color];
                    // Now write it back out.
                    spriteImage.setPixelColor(color, baseX + x, baseY + y);
                }
            }
        }
        

        verbose('Writing image...');
        spriteImage.write(spriteOutFile);
        out('Success! Image written to ' + spriteOutFile);


    });
});
    