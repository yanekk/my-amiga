#include "bounce.h"

void Bounce_Update(struct Bounce* current) {
    if(current->y == 0)
        current->jump = JUMP_STRENGHT;
    current->y += current->jump;

    if(++current->gravityFrames == GRAVITY_FRAMES) {
        current->jump -= 1; 
        current->gravityFrames = 0;
    }
}