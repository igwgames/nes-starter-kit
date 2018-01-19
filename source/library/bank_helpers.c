#include "source/library/bank_helpers.h"


void banked_call(unsigned char bankId, void (*method)(void)) {
    // FIXME: Have a short queue, and a pointer to it.
    set_prg_bank(bankId);
    (*method)();
}