# TMX2C 

TMX2C is a simplistic project used to turn Tiled map files into C data that can be used
with an NES game engine. (And possibly other systems.)

For now, it is meant to be used exclusively with nes-starter-kit. It is probably possible
to expand to general C development if desired. This project currently lacks documentation,
and is served up as-is. The nes-starter-kit documentation should explain how to use it 
in that context.

In short, pass in a tmx file, get a C file with the data in response.

TODO: Document this way better
TODO: Package this and distribute with nes-starter-kit binaries.
TODO: Is it worth adding this to the makefile? We can call cmd from bash if we need to...

Building: `pkg -d --public tools/tmx2c/src/index.js --output tools/tmx2c/tmx2c.exe -t node6-windows-x64`