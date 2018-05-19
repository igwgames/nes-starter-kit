/**
 * This is a quick and dirty script used to parse chr files into modern image formats. 
 * It is compilable into a binary for various operating systems, so nodejs isn't 
 * required! 
 * 
 * It's still very quick and dirty. I tried to document some of it, but it probably isn't very clear.
 * Don't be afraid to reach out for help if you're tweaking this. I hope it meets most needs as-is.
 */
var VERSION = require('package.json').version;

// Expects exactly (binary) infile outfile (first param is always node)
if (process.argv.length != 5) {
    printUsage();
    process.exit(1);
}

function printDate() {
    return '[' + new Date().toUTCString() + '] ';
}

function printUsage() {
    out('chr2img version ' + VERSION);
    out('Usage: chr2img [chr file to convert] [pal file to convert] [file to save output to]');
}

function out() {
    var args = [].slice.call(arguments);
    args.unshift('[chr2img] ', printDate());

    console.info.apply(this, args);
}

function verbose() {
    var args = [].slice.call(arguments);
    args.unshift('[chr2img] ', printDate());

    if (logLevel == 'verbose') {
        console.info.apply(this, args);
    }
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

// Bunch of variables. Hopefully the names are semi clear...
var fs = require('fs'),
    Jimp = require('jimp'),
    chrFile = process.argv[2],
    palFile = process.argv[3],
    outFile = process.argv[4],
    chrContents = fs.readFileSync(chrFile),
    palContents = fs.readFileSync(palFile),
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

// Create a new image, at the size our tile editor expects it.
new Jimp(128, 512, function(err, image) {
    image.rgba(false);
    // See: http://wiki.nesdev.com/w/index.php/PPU_pattern_tables
    // Here's the bulk of what's going on... we loop over the chr file, and translate it to an image. 
    // Apologies, I'm not sure how to document this better...

    for (var tileRowId = 0; tileRowId < 16; tileRowId++) { // Loop over rows
        for (var tileId = 0; tileId < 16; tileId++) { // Loop over every tile in row 1
            for (var rowId = 0; rowId < 8; rowId++) { // Loop over every row in the tile
                for (var pixelId = 0; pixelId < 8; pixelId++) { // And every bit. (I'm sorry)
                    var colorId = (chrContents[(tileRowId * 256) + (tileId*16)+rowId] & reverseBitmaskLookup[pixelId] ? 1 : 0) +  (chrContents[(tileRowId * 256) + (tileId*16)+rowId+8] & reverseBitmaskLookup[pixelId] ? 2 : 0);

                    image.setPixelColor(rgbPalettes[0][colorId], tileId*8 + pixelId, tileRowId*8 + rowId);
                    image.setPixelColor(rgbPalettes[1][colorId], tileId*8 + pixelId, tileRowId*8 + rowId + 128);
                    image.setPixelColor(rgbPalettes[2][colorId], tileId*8 + pixelId, tileRowId*8 + rowId + 256);
                    image.setPixelColor(rgbPalettes[3][colorId], tileId*8 + pixelId, tileRowId*8 + rowId + 384);

                }
            }
        }
    }

    // Okay, we made some image science. Dump it on out into a file!
    image.write(outFile);
});
    

