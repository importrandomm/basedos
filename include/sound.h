#ifndef SOUND_H
#define SOUND_H

#include <sys/types.h>

// Play a sound with the specified frequency
void beep(uint32_t frequency);

// Stop the sound
void nosound(void);

// Play a startup sound
void play_startup_sound(void);

#endif // SOUND_H
