# sprite_def2img

Converts a sprite definition, alongside a palette and chr file, and generates a 
png file with the sprites suitable for use with Tiled.

## Command

Run the command as follows:

```
 sprite_def2img  [path/to/sprite_definitions.c] [path/to/main_sprite.chr] [path/to/main_sprite.pal] [path/to/output.png]

 tmx2c 4 overworld levels/overworld.tmx test
 ```

## Packaging for nes-starter-kit

We package it using a node module called `pkg`, which is installed globally. (`npm install -g pkg`)

Use the following command to build a release exe:

```
pkg -d --public tools/sprite_def2img/src/index.js --output tools/sprite_def2img/sprite_def2img.exe -t node8-windows-x64
```

Note: This must be run from the root nes-starter-kit folder.