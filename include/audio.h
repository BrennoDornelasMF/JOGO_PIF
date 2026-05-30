#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

void InitAudio_Game(void);
void UpdateAudio(void);
void UnloadAudio_Game(void);


void InitCarSounds(void);
void UpdateCarSounds(float speed, float maxSpeed, bool isBraking);
void UnloadCarSounds(void);

#endif