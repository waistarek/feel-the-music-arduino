#include "sound_to_vibe.h"
#include "light_mapping.h"
#include <Adafruit_DRV2605.h>
#include <SD.h>
#include <SPI.h>

#define MAX_VIBE_EVENTS 700

Adafruit_DRV2605 drv;

// Struktur zur Speicherung eines Vibrationsevents
struct VibeEvent {
  uint32_t time;   // Zeitpunkt in ms
  uint8_t pattern; // Vibrationsmuster
};

VibeEvent vibeEvents[MAX_VIBE_EVENTS];
int vibeEventCount = 0;
unsigned long startTime_vibe = 0;
int currentIndex_vibe = 0;

extern bool systemAktiv; // Verknüpft mit globalem EIN/AUS-Zustand

// Liest die CSV-Datei mit Vibrationsevents
bool loadVibrationMapping(const char* filename) {
  File f = SD.open(filename);
  if (!f) {
    Serial.print("❌ Datei nicht gefunden: ");
    Serial.println(filename);
    return false;
  }

  Serial.print("📄 Lade Vibrations-Mapping: ");
  Serial.println(filename);
  String header = f.readStringUntil('\n');

  vibeEventCount = 0;
  while (f.available() && vibeEventCount < MAX_VIBE_EVENTS) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;

    int sep = line.indexOf(',');
    vibeEvents[vibeEventCount].time = line.substring(0, sep).toInt();
    vibeEvents[vibeEventCount].pattern = line.substring(sep + 1).toInt();
    vibeEventCount++;
  }

  f.close();

  Serial.print("✅ Geladene Vibrationsevents: ");
  Serial.println(vibeEventCount);
  return true;
}

// Initialisiert das Vibrationsmodul
void setupVibration() {
  drv.begin();
  drv.selectLibrary(1);
  drv.setMode(DRV2605_MODE_INTTRIG);
  startTime_vibe = millis();
  currentIndex_vibe = 0;
}

// Führt zeitlich gesteuerte Vibrationen aus
void updateVibration() {
  if (!systemAktiv) return; // Nur aktiv, wenn System eingeschaltet ist
  if (currentIndex_vibe >= vibeEventCount) return;

  unsigned long now = millis() - startTime_vibe;

  while (currentIndex_vibe < vibeEventCount && now >= vibeEvents[currentIndex_vibe].time) {
    uint8_t pattern = vibeEvents[currentIndex_vibe].pattern;
    drv.setWaveform(0, pattern);
    drv.setWaveform(1, 0);
    drv.go();
    currentIndex_vibe++;
  }
}

void resetVibration() {
  currentIndex_vibe = 0;
}

