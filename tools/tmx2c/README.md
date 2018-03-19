# TMX2C 

TMX2C is a simplistic project used to turn Tiled map files into C data that can be used
with an NES game engine. (And possibly other systems.)

For now, it is meant to be used exclusively with nes-starter-kit. It is probably possible
to expand to general C development if desired. This project currently lacks documentation,
and is served up as-is. The nes-starter-kit documentation should explain how to use it 
in that context.

In short, pass in a tmx file, get a C file with the data in response.

## Command

Run the command as follows:

```
 tmx2c [bank number] [variable name] [file to convert] [file to save output to, no extension]

 tmx2c 4 overworld levels/overworld.tmx test
 ```

## Packaging for nes-starter-kit

We package it using a node module called `pkg`, which is installed globally. (`npm install -g pkg`)

Use the following command to build a release exe:

```
pkg -d --public tools/tmx2c/src/index.js --output tools/tmx2c/tmx2c.exe -t node8-windows-x64
```

Note: This must be run from the root nes-starter-kit folder.