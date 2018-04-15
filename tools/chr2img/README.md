# chr2img

chr2img is a quick little nodejs app that converts a given chr file and palette into a large
128x512 png image suitable for use with tile editors. (Such as Tiled) This program expects the
format exported by NES Screen Tool, but it is likely that other programs will also work.

## Command

Run the command as follows: 

```
chr2img [chr file to convert] [pal file to convert] [file to save output to

chr2img graphics/tiles.chr graphics/palettes/main_bg.pal ./test.png
```

## Packaging for nes-starter-kit

We package it using a node module called `pkg`, which is installed globally. (`npm install -g pkg`)

Use the following command to build a release exe:

```
pkg -d --public tools/chr2img/src/index.js --output tools/chr2img/chr2img.exe -t node8-windows-x64
```

Note: This must be run from the root nes-starter-kit folder.
