#pragma once

// Lädt die CSV-Datei (z. B. "001_vibe.csv") mit Vibrationsmappingdaten
bool loadVibrationMapping(const char* filename);

// Initialisiert das Vibrationsmodul (DRV2605L)
void setupVibration();

// Aktualisiert die Vibration zur passenden Zeit basierend auf dem Mapping
void updateVibration();
void resetVibration();  // Deklaration ergänzen
void resetAllEffects();