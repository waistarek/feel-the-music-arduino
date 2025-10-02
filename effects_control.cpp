#include <Arduino.h>
#include "effects_control.h"
#include "light_mapping.h"
#include "sound_to_vibe.h"

// Globale Variablen aus den anderen Modulen
extern unsigned long startTime;
extern int currentIndex;

extern unsigned long startTime_vibe;
extern int currentIndex_vibe;

void resetAllEffects() {
  startTime = millis();
  currentIndex = 0;

  startTime_vibe = startTime; // Synchronisierte Startzeit
  currentIndex_vibe = 0;

  resetLights();
  resetVibration();
}

