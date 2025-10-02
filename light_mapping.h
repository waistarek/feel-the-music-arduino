#pragma once

// Lädt die CSV-Datei (z. B. "001_light.csv") mit den Mappingdaten
bool loadLightMapping(const char* filename);

// Initialisiert den LED-Streifen (z. B. Adafruit NeoPixel)
void setupLights();

// Aktualisiert das Licht zur passenden Zeit basierend auf dem Mapping
void updateLights();
void resetLights();  // Deklaration ergänzen
