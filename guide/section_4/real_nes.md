# Playing your game on a Real NES Console

Okay, you've spent a bunch of effort making a game for an old console; wouldn't you like to try
it on that console? Well, you can! It's not free, but you can probably do it for about $50 if you
have a little patience to learn.

A few simple options for this are catridges from InfiniteNesLives, and a flash cartridge such as the 
PowerPak by retroUSB. There are other suppliers available as well, though more work will be required. 
These are detailed at the bottom of this chapter.

The INL cartridge is cheaper and essentially real NES hardware - however it will only work for games with the same mapper as yours. (MMC1 by default)

In contrast, a flash cartridge is a lot more expensive (about double the price) and a little further
from real hardware. That said, this is a flash cartridge that will be capable of playing almost
any NES game. (Including commercial NES titles, if you have dumped the rom legally.) This will
also be compatible if you decide to change things like the mapper for your game. Finally, the
PowerPak is a lot easier to work with.

Both are fantastic pieces of hardware; choose whichever one better suits what you want to do.

## InfiniteNesLives Cartridge

#### Important Note on INL Cartridges

Unfortunately, since the writing of this guide, Paul has taken down the InfiniteNesLives cartridge 
parts from his store. You may have luck reaching out to him directly for help, however
you may also need to resort to other options. His contact information is available on
the store website.

The way to flash cartridges has also changed significantly, though I think the old
flashing tool is likely still around. The new tool works well, but as cartridges are
a bit hard to come by, I haven't taken time to document how to use it yet.

#### Using INL Cartridges

The engine is built to be compatible with the MMC1 mapper, and by default uses 128k of PRG rom, 
and up to 128k of chr rom. This makes it a perfect match for the 
[SKRom boards by InfiniteNesLives](http://www.infiniteneslives.com/nessupplies.php#SKROM). 
I suggest going for the 128kb model with battery backing, but the 256kb model can be made
to work if you double your rom size to 256kb, as described in a later chapter.

You will also need to purchase a ROM flashing tool in order to use this. It works like this: 
you plug the flashing device into your computer via usb, then  you plug your NES cartridge
into the flashing device, and run a special program to write your game's rom to the cartridge.

[Flashing tool on InfiniteNesLives](http://www.infiniteneslives.com/kazzo.php)

Finally, you should also purchase a cartridge shell to put the board into, unless you want to use one
from an existing NES game. InfiniteNesLives also has these available 
[here](http://www.infiniteneslives.com/nessupplies.php#cases). (Bonus points: you can pick a color other
than gray!)

Once you have these, you will need two pieces of software to do it: a rom splitter, and 
a flash program. Here are the two I use: 

Rom Splitter: 
- http://kevinselwyn.com/ReadNES.js/ (online)
- https://www.romhacking.net/?page=utilities&action=images&id=683&imageid=title (windows)

Flasher: 
- http://www.infiniteneslives.com/kazzo.php (Look for the "software/firmware package" link at the bottom)

Got the software downloaded and open? Awesome! First, take your rom from the `nes-starter-kit/rom` folder 
(`starter.nes` or similar) And put this into your rom splitting program. If you use the online one, it'll 
do everything  for you, and give you a zip file with a .prg and .chr file. If you use the other one, be 
sure to check off the "Remove 16 Byte Nes Header" and "Output CHR / PRG" options, and do "Auto Split 
CHR/PRG". The end result should be about the same.

Next, open "INL-Retro Prog" from the Flasher zip file, then follow the instructions in the `readme.txt` 
at the Kazzo link above, past this point. The readme explains the steps rather clearly as long as you
have the split files. Either rename the files from .chr and .prg to .bin, or choose "All Files (.*)" 
from the file selector in INL-Retro Prog. Otherwise, the steps there should hopefully be clear enough.


## PowerPak (or Everdrive)

The engine is completely compatible with the PowerPak, as most NES games are. You can get your
hands on the hardware from [RetroUSB](https://www.retrousb.com/product_info.php?products_id=34).

You will also need to find a CompactFlash card reader, and either order it with a CompactFlash
card included, or buy your own. You should be able to find these on Amazon pretty easily. 

Once you have them, take the CompactFlash card out of your PowerPak, and plug it into your 
card reader. from Windows explorer, copy the rom from the `rom/` folder onto the CompactFlash
card. Now plug the CompactFlash card back into your NES, and turn it on. Just pick your rom
file from the menu, and you're good. Enjoy!

There is also another option called the Everdrive - I don't have the NES one myself, however I have had 
great luck with their cartridges for other consoles. It's worth checking out.
[Everdrive retailer](https://krikzz.com/)

## Other Cartridge hardware retailers

It is also very possible to create a NES cart using more raw hardware. You probably won't have the
same reflashing abilities as the InfiniteNesLives cartridges, but you can build a cartridge. Some
people also find this kind of fun!

You want to build an `MMC1 SKROM` cartridge, unless you have changed the mapper at some point.

[Here's a guide](https://mousebitelabs.com/2021/01/27/nes-reproduction-board-guide-mmc1/) on how to assemble an MMC1 cartridge from MouseBiteLabs. 
They also have the circuit board available 
[on their Etsy page](https://www.etsy.com/shop/MouseBiteLabs). 
You will need to buy the other components used for the cartridge and solder them, however.

[Muramasa Entertainment](https://www.muramasaentertainment.com/product/nes-txsxrom-advanced-mmc1/)
also has circuit boards, though these do not have an associated guide. (MouseBiteLabs' guide may work,
but this author has not tested it.)
