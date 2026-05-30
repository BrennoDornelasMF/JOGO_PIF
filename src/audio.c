#include "audio.h"
#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Liste aqui todos os arquivos da sua playlist
static const char* tracks[] = {
    "musicGame/01.mp3",
    "musicGame/02.mp3",
    "musicGame/03.mp3",
    "musicGame/04.mp3",
    "musicGame/05.mp3",
    "musicGame/06.mp3",
    "musicGame/07.mp3",
    "musicGame/08.mp3",
    "musicGame/09.mp3",
    "musicGame/10.mp3",
    "musicGame/11.mp3",
    "musicGame/12.mp3",
    "musicGame/13.mp3",
    "musicGame/14.mp3",
    "musicGame/15.mp3",
    "musicGame/16.mp3",
    "musicGame/17.mp3",
    "musicGame/18.mp3", 
    "musicGame/19.mp3",      
};

#define TRACK_COUNT (sizeof(tracks) / sizeof(tracks[0]))

static Sound sndEngine;
static Sound sndBrake;
static Music currentMusic;
static int   currentTrack = -1;

static void PlayRandomTrack(void) {
    // Para a música atual se estiver tocando
    if (currentTrack >= 0)
        UnloadMusicStream(currentMusic);

    // Escolhe uma diferente da atual
    int next;
    do {
        next = rand() % TRACK_COUNT;
    } while (next == currentTrack && TRACK_COUNT > 1);

    currentTrack  = next;
    currentMusic  = LoadMusicStream(tracks[currentTrack]);
    SetMusicVolume(currentMusic, 0.3f);
    PlayMusicStream(currentMusic);
}

void InitAudio_Game(void) {
    InitAudioDevice();
    srand((unsigned int)time(NULL));  // seed aleatória
    PlayRandomTrack();
}

void UpdateAudio(void) {
    // Necessário chamar todo frame para manter o stream
    UpdateMusicStream(currentMusic);

    // Quando a música terminar, toca outra aleatória
    if (!IsMusicStreamPlaying(currentMusic))
        PlayRandomTrack();
}

void InitCarSounds(void) {
    sndEngine = LoadSound("musicGame/engine.ogg");
    sndBrake  = LoadSound("musicGame/tire.ogg");
    SetSoundVolume(sndEngine, 1.0f);
    SetSoundVolume(sndBrake,  0.9f);
    PlaySound(sndEngine); // começa o loop do motor
}

void UpdateCarSounds(float speed, float maxSpeed, bool isBraking) {
    float t      = speed / maxSpeed;           // 0.0 → 1.0
    float pitch  = 0.5f + t * 1.5f;           // 0.5 (idle) → 2.0 (full)
    float volume = 0.7f + t * 0.3f;

    if (isBraking && speed > 0.3f) {
        // motor fica grave ao frear
        SetSoundPitch(sndEngine, 0.35f);
        SetSoundVolume(sndEngine, 0.8f);

        if (!IsSoundPlaying(sndBrake))
            PlaySound(sndBrake);
    } else {
        StopSound(sndBrake);
        SetSoundPitch(sndEngine, pitch);
        SetSoundVolume(sndEngine, volume);
    }

    // mantém o loop do motor vivo
    if (!IsSoundPlaying(sndEngine))
        PlaySound(sndEngine);
}

void UnloadCarSounds(void) {
    UnloadSound(sndEngine);
    UnloadSound(sndBrake);
}

void UnloadAudio_Game(void) {
    UnloadCarSounds();
    UnloadMusicStream(currentMusic);
    CloseAudioDevice();
}