/**
 * NES Starter Kit Tiled Extension
 * 
 * This mainly exists to show errors when tiles are placed on the sprite layer, or sprites are placed on the tile layer.
 * 
 * I'd like to just stop the placement entirely, but right now the extension library in Tiled isn't quite powerful enough.
 */

(function() {

    function log(msg) {
        tiled.log('[nes-starter-kit] ' + msg);
    }

    log('NES Starter Kit extension initialized');

    function FixNesStarterKitMap(action, fix) {
        if (tiled.activeAsset === null) {
            return;
        } else if (!tiled.activeAsset.isTileMap) {
            tiled.alert('This extension only knows how to activate on maps. Please switch to the map you want to use with nes-starter-kit.');
            return;
        }
        var layers = {},
            layerArray = [],
            firstTileLayer = null,
            firstSpriteLayer = null;
        for (var i = 0; i < tiled.activeAsset.layerCount; i++) {
            var layer = tiled.activeAsset.layerAt(i);
            layers[layer.name] = layer;
            layerArray.push(layer);

            if (!firstTileLayer && layer.name.toLowerCase().indexOf('tile') !== -1) {
                firstTileLayer = layer;
            } else if (!firstSpriteLayer && layer.name.toLowerCase().indexOf('sprite') !== -1) {
                firstSpriteLayer = layer;
            }
        }

        var firstTileTileset = null;
        for (var i = 0; i < tiled.activeAsset.tilesets.length; i++) {

            if (tiled.activeAsset.tilesets[i].name.toLowerCase().indexOf('tile') !== -1) {
                firstTileTileset = tiled.activeAsset.tilesets[i];
                break;
            }
        }

        if (!layers['Overworld Tiles'] || !layers['Sprites']) {
            log('This does not appear to be a nes-starter-kit based map... it is missing either the "Overworld Tiles" layer or the "Sprites" layer. Skipping.');
            return;
        }

        if (firstTileTileset === null) {
            log('This does not appear to have any map/overworld tilesets - nes-starter-kit extension is unable to function. Skipping.');
            return;
        }

        layerArray.forEach(function(layer) {
            var compareWith = null,
                msgCompareWith = null,
                msgIs = null,
                sendTo = null,
                replaceTile = null;
            if (layer.name.toLowerCase().indexOf('sprite') !== -1) {
                compareWith = 'tile';
                msgCompareWith = 'Tile';
                msgIs = 'Sprite';
                sendTo = firstTileLayer;
                replaceTile = null;
            } else if (layer.name.toLowerCase().indexOf('tile') !== -1) {
                compareWith = 'sprite';
                msgCompareWith = 'Sprite';
                msgIs = 'Tile';
                sendTo = firstSpriteLayer;
                replaceTile = firstTileTileset.tile(0);
            } else {
                log('Skipping layer named "' + layer.name + '" - not a tile or sprite layer.');
                return;
            }

            function doFix(origTile, lookupObj) {
                var editable = sendTo.edit();
                editable.setTile(lookupObj.x, lookupObj.y, origTile);
                editable.apply();

                var otherEditable = layer.edit();
                otherEditable.setTile(lookupObj.x, lookupObj.y, replaceTile);
                otherEditable.apply();
            }

            
            for (var x = 0; x < layer.width; x++) {
                for (var y = 0; y < layer.height; y++) {
                    if (layer.tileAt(x, y) && layer.tileAt(x, y).tileset.name.toLowerCase().indexOf(compareWith) !== -1) {
                        var origTile = layer.tileAt(x, y);
                        if (fix) {
                            var lookupObj = {
                                x: x,
                                y: y
                            };
                            doFix(origTile, lookupObj);
                            log(msgIs + ' on layer "' + layer.name + '" at (' + lookupObj.x + ', ' + lookupObj.y + ') appears to be a ' + msgCompareWith + '. This should be moved to a ' + msgCompareWith + ' layer. Double-click to move it to "' + sendTo.name + '".');
                        } else {
                            var lookupObj = {
                                x: x,
                                y: y
                            };
                            tiled.warn('[nes-starter-kit] ' + msgIs + ' on layer "' + layer.name + '" at (' + lookupObj.x + ', ' + lookupObj.y + ') appears to be a ' + msgCompareWith + '. This should be moved to a ' + msgCompareWith + ' layer. Double-click to move it to "' + sendTo.name + '".', function() {
                                doFix(origTile, lookupObj);
                            });
                        }
                    }
                }
            }
        });
    }

    var cleanupMap = tiled.registerAction('FixNesStarterKitMap', function(action) {
        FixNesStarterKitMap(action, true);
    });

    cleanupMap.text = 'Clean Up NES Starter Kit map';
    cleanupMap.shortcut = 'Ctrl+Shift+K';
    cleanupMap.toolTip = 'Moves all sprite tiles to the sprite layer, and tiles to the tile layer automatically.';
    cleanupMap.statusTip = cleanupMap.toolTip;

    var warnAboutMap = tiled.registerAction('WarnNesStarterKitFixes', function(action) {
        if (warnAboutMap.checked) {
            FixNesStarterKitMap(null, false);
        }
    });
    warnAboutMap.text = 'Show NES Starter Kit Warnings';
    warnAboutMap.checkable = true;
    warnAboutMap.checked = true;

    tiled.activeAssetChanged.connect(function(asset) {
        if (warnAboutMap.checked) {
            FixNesStarterKitMap(null, false);
        }
    });

    tiled.assetOpened.connect(function(asset) {
        if (warnAboutMap.checked) {
            if (warnAboutMap.checked) {
                FixNesStarterKitMap(null, false);
            }
        }
    });

    tiled.assetSaved.connect(function(asset) {
        FixNesStarterKitMap(null, false);
    });

    tiled.extendMenu('Map', [
        {separator: true},
        {action: 'FixNesStarterKitMap'},
        {action: 'WarnNesStarterKitFixes'}
    ]);

})();