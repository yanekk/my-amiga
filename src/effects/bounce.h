#include <exec/types.h>
#define JUMP_STRENGHT 5
#define GRAVITY_FRAMES 5

struct Bounce {
    SHORT y;
    SHORT jump;
    SHORT gravityFrames;
};

void Bounce_Update(struct Bounce* current);