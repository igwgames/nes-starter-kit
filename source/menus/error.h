// Stuff involved with showing an error page if things go wrong in an unexpected way.
// Hopefully a little more helpful than the rom hanging, sometimes.

// Known error states (defined in error.c) for things to use.
// Note that you do not *have* to use these constants if you want to show other errors. They are just strings.
extern const char* ERR_UNKNOWN_GAME_STATE;
extern const char* ERR_UNKNOWN_GAME_STATE_EXPLANATION;
extern const char* ERR_RECURSION_DEPTH;
extern const char* ERR_RECURSION_DEPTH_EXPLANATION;

// What bank do we wanna put this stuff in?
#define PRG_BANK_ERROR 1

// Spit out an error screen and crash the rom.
// errorId: A string describing the error. Use one of the constants in error.c/here
// errorDescription: Some text describing the error in detail. Alternatively, pass NULL.
// numberName: A description of the numeric parameter accepted after this one. Can be NULL.
// number: A number to print out that goes with the error code (address, something else...) Can be NULL.
void crash_error(const char* errorId, const char* errorDescription, const char* numberName, int number);