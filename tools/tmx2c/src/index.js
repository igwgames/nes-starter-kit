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

// Expects exactly tmx2c infile outfile (first param is always node)
if (process.argv.length != 6) {
    printUsage();
    process.exit(1);
}


var tmxParse = require('tmx-parser'),
    fs = require('fs'),
    logLevel = 'info', // change to 'verbose' for some extra output
    tileMap = null,
    spriteMap = null,
    file = process.argv[4],
    SCREEN_WIDTH = 16,
    SCREEN_HEIGHT = 12,
    SCREEN_HEIGHT_PADDED = 16,
    outFile = process.argv[5] + '.c',
    outHeader = process.argv[5] + '.h',
    name = null,
    outputString = '';

function printDate() {
    return '[' + new Date().toUTCString() + '] ';
}

function printUsage() {
    out('tmx2c version ' + VERSION);
    out('Usage: tmx2c [bank number] [variable name] [file to convert] [file to save output to]');
}

function out() {
    var args = [].slice.call(arguments);
    args.unshift('[tmx2c] ', printDate());

    console.info.apply(this, args);
}

function verbose() {
    var args = [].slice.call(arguments);
    args.unshift('[tmx2c] ', printDate());

    if (logLevel == 'verbose') {
        console.info.apply(this, args);
    }
}

tmxParse.parseFile(process.argv[4], function(err, tmxData) {
    if (err || !tmxData) {
        out('Failed parsing TMX file - is it corrupt?', err, tmxData);
        process.exit(1);
    }

    verbose('Parsed ', process.argv[4], '. Writing to ', outFile, ' and ', outHeader, '...');

    verbose('args', process.argv);
    // verbose('raw data', tmxData);

    var width = tmxData.width,
        height = tmxData.height,
        roomsWide = width / SCREEN_WIDTH,
        roomsTall = height / SCREEN_HEIGHT,
        data = null,
        spriteData = null,
        originalSize = null,
        rawColumns = [],
        columnDefinitions = [],
        mapColumns = [],
        fileData = '',
        mapData = '',
        spriteData = '',
        containsWarnings = false;

    verbose('Map width: ' + width + ' height: ' + height + ' length: ' + (width * height) + ' bytes.');
        

    for (var i = 0; i < tmxData.layers.length; i++) {
        if (tmxData.layers[i].name == 'Overworld Tiles') {
            data = tmxData.layers[i].tiles;
        } else if (tmxData.layers[i].name == 'Sprites') {
            spriteData = tmxData.layers[i].tiles;
        }
    }

    if (data == null || spriteData == null) {
        out('Failed parsing map - did you remove or rename the "Overworld Tiles" or "Sprite" layer?');
        process.exit(1);
    }

    originalSize = data.length;

    // Loop through all rooms
    for (var y = 0; y < roomsTall; y++) {
        for (var x = 0; x < roomsWide; x++) {
            var roomSpriteData = [];
            if (x == 7 && y == 7) {
                verbose('Skipping the final room, because there is not enough rom space the way we map things out. Perhaps if we used RLE...');
                continue; // Skips this single iteration, but continues the loop.
            }
            mapData += "\n\n//Room (" + x + ":" + y + ")\n\n// Room data\n"

            // loop through every tile in the room.
            for (var yy = 0; yy < SCREEN_HEIGHT; yy++) {
                for (var xx = 0; xx < SCREEN_WIDTH; xx++) {
                    if (xx != 0) {
                        mapData += ', '
                    } else if (yy != 0) {
                        mapData += ",\n";
                    } else {
                        mapData += "\n";
                    }

                    var pos = (x * SCREEN_WIDTH) + (y * (roomsWide*SCREEN_WIDTH)*SCREEN_HEIGHT) + (yy * width) + xx;
                    if (data[pos].gid < 0 || data[pos].gid > 256) {
                        containsWarnings = true;
                        out('WARNING: Sprite (id: ' + data[pos].gid + ') found on map layer in room ( ' + x + ', ' + y + ') - this sprite will be skipped! Please move it to the sprite layer.');
                        data[pos].gid = 0;
                    }
                    mapData += data[pos].gid - 1;

                    if (spriteData[pos]) {
                        if (spriteData[pos].gid < 256) {
                            out('WARNING: Map tile (id: ' + spriteData[pos].gid + ') found on sprite layer in room ( ' + x + ', ' + y + ') - this tile will be skipped! Please move it to the tile layer.');
                            containsWarnings = true;
                        } else {
                            roomSpriteData.push(yy*SCREEN_WIDTH+xx);
                            roomSpriteData.push(spriteData[pos].gid-257);
                        }
                    }
    
                }
            }
            mapData += ","
            // Pad roomSpriteData to 8 sprites, and add a bit of padding to max out to 32 bytes.
            if (roomSpriteData.length > 16) {
                containsWarnings = true;
                out('WARNING: Truncating room (' + x + ',' + y + ') to max 12 sprites. The engine cannot support more; please clean some up.');
                roomSpriteData.length = 32;
            }
            while (roomSpriteData.length < 32) {
                roomSpriteData.push(255);
            }
            
            // Okay, main room data is done, add sprites
            mapData += "\n\n// Sprite Data\n";
            for (var i = 0; i < 15; i++)
                mapData += roomSpriteData[i] + ', ';
            mapData += roomSpriteData[15] + ",\n";
            for (var i = 16; i < 31; i++)
                mapData += roomSpriteData[i] + ', ';
            mapData += roomSpriteData[31]+ ",\n";

            // Lastly, we want things to line up perfectly so we have 256 per map tile (makes math easier)
            // So, add some padding. (Note: If you wanna add your own data, this is the spot!)
            mapData += '// add a little padding - we want each map screen to take up exactly 256 bytes to make math easier.\n';
            mapData += '0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \n';
            mapData += '0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \n\n';

        }
    }

    // Kill the very last comma
    mapData = mapData.replace(/,\s*$/, '');
    // Add a note about 7,7 to the acutal output, to make it clear.
    mapData += "\n\n// Room (7:7) skipped due to space constraints\n\n";
    // Need to include the count of characters to make C happy :(
    var elementCount = mapData.split(',').length;



    // Little trick to tab every single line in, so our output looks nicer
    mapData = "    " + mapData.replace(/[\n]/g, "\n    ");

    mapData = "#include \"source/library/bank_helpers.h\"\n#include \""+process.argv[5]+".h\"\n\nCODE_BANK(PRG_BANK_MAP_"+process.argv[3].toUpperCase()+");\n\n"+'const unsigned char ' + process.argv[3] + "["+elementCount+"] = {\n\n" + mapData + "\n\n\n};\n";
    var headerData = "// This is the data for your entire map, as made available in the .c file of this name\n\n#define PRG_BANK_MAP_"+process.argv[3].toUpperCase()+" "+process.argv[2]+"\nextern const unsigned char "+process.argv[3]+"[" + elementCount + "];";

    fs.writeFileSync(process.argv[5]+'.c', mapData);
    fs.writeFileSync(process.argv[5]+'.h', headerData);

    if (containsWarnings) {
        out('WARNING: Map conversion completed with warnings. Please look over the logs above to find them.')
    } else {
        out('Map conversion completed successfully.')
    }
});