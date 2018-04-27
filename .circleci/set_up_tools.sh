# Install cc65 if it's not already in the cache.
# Grab it from the interbuttz, then extract it and build it using gcc 
mkdir -p tools/cc65/bin
if [ ! -f tools/cc65/bin/cc65 ]; then mkdir -p tools/cc65-temp; fi
if [ ! -f tools/cc65/bin/cc65 ]; then cd tools/cc65-temp/ && wget ftp://ftp.musoftware.de/pub/uz/cc65/cc65-sources-2.13.3.tar.bz2 && tar xvjf cc65-sources-2.13.3.tar.bz2 && cd ../..; fi
if [ ! -f tools/cc65/bin/cc65 ]; then cd tools/cc65-temp/cc65-2.13.3 && make --file=make/gcc.mak && cd ../../..; fi
if [ ! -f tools/cc65/bin/cc65 ]; then mv tools/cc65-temp/cc65-2.13.3/* tools/cc65; fi
# Create a bin dir, then copy all the files to it.
find tools/cc65 -type f -executable -exec cp {} tools/cc65/bin/ \;
