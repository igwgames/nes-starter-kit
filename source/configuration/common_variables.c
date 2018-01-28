// This file contains a bunch of general purpose variables, so that they can share one memory
// location (usually in zeropage) and not be defined in a bunch of places.
#include "source/library/bank_helpers.h"

ZEROPAGE_EXTERN(unsigned char, i);
ZEROPAGE_EXTERN(unsigned char, j);