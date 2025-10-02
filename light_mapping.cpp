#include "light_mapping.h"
#include "sound_to_vibe.h"
#include <Adafruit_NeoPixel.h>
#include <SD.h>
#include <SPI.h>

// Der LED-Streifen ist an diesem Pin angeschlossen (z. B. D6)
#define LED_PIN 6
#define NUM_PIXELS 30

// Wir begrenzen die maximale Anzahl an Lichteffekten auf 1000
#define MAX_EVENTS 700

// Initialisiere den LED-Streifen
Adafruit_NeoPixel pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Definiere eine Struktur für jeden Lichteffekt aus der CSV
struct LightEvent {
  uint32_t time;  // Zeit in Millisekunden
  uint8_t r, g, b; // Rot, Grün, Blau
};

// Array für alle eingelesenen Events
LightEvent events[MAX_EVENTS];
int eventCount = 0;               // Zähler für geladene Events
unsigned long startTime = 0;     // Zeitpunkt des Starts
int currentIndex = 0;            // aktueller Index im Ereignisarray

extern bool systemAktiv; // Systemstatus prüfen

// Liest die CSV-Datei und speichert Lichtereignisse ins RAM
bool loadLightMapping(const char* filename) {
  File f = SD.open(filename); // Öffne Datei von SD-Karte
  if (!f) {
    Serial.print("❌ Datei nicht gefunden: ");
    Serial.println(filename);
    return false;
  }

  Serial.print("📄 Lade Licht-Mapping: ");
  Serial.println(filename);

  // Erste Zeile (Kopfzeile) überspringen
  String header = f.readStringUntil('\n');

  eventCount = 0;
  while (f.available() && eventCount < MAX_EVENTS) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue; // leere Zeile überspringen

    // Spaltenpositionen suchen
    int t1 = line.indexOf(',');
    int t2 = line.indexOf(',', t1 + 1);
    int t3 = line.indexOf(',', t2 + 1);

    // Werte extrahieren und speichern
    events[eventCount].time = line.substring(0, t1).toInt();
    events[eventCount].r    = line.substring(t1 + 1, t2).toInt();
    events[eventCount].g    = line.substring(t2 + 1, t3).toInt();
    events[eventCount].b    = line.substring(t3 + 1).toInt();
    eventCount++;
  }

  f.close();

  Serial.print("✅ Geladene Licht-Ereignisse: ");
  Serial.println(eventCount);
  return true;
}

// Vorbereitung: LED-Streifen einschalten und Startzeit setzen
void setupLights() {
  pixels.begin();
  pixels.show(); // schaltet alle LEDs aus
  startTime = millis();
  currentIndex = 0;
}

// Hauptfunktion: prüft, ob ein neues Licht-Ereignis fällig ist
void updateLights() {
  if (!systemAktiv) return; // Nur aktiv, wenn System eingeschaltet ist
  if (currentIndex >= eventCount) return; // Keine Events mehr übrig

  unsigned long now = millis() - startTime;

  // Prüfe, ob aktuelles Ereignis fällig ist
  while (currentIndex < eventCount && now >= events[currentIndex].time) {
    // Farbe setzen laut Mapping
    for (int i = 0; i < NUM_PIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(
        events[currentIndex].r,
        events[currentIndex].g,
        events[currentIndex].b
      ));
    }
    pixels.show();

    currentIndex++;
  }
}
void resetLights() {
  pixels.clear();
  pixels.show();
  currentIndex = 0;
}

