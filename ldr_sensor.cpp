#include "ldr_sensor.h"
#include <Arduino.h>

#define LDR_PIN A0  // Sensor an Pin A0
#define THRESHOLD 500 //Schwellenwert
int referenzHelligkeit = 0;   // Umgebungshelligkeit
float faktor = 0.8;            // Schwellenwert = 80% der Referenz
unsigned long letzteKalibrierung = 0;
const unsigned long kalibrierIntervall = 5UL * 60UL * 1000UL; // 5 Minuten in ms

void setupLDR() {
  pinMode(LDR_PIN, INPUT);
  kalibriereLDR();
}

void kalibriereLDR() {
  long summe = 0;
  const int anzahlMessungen = 10;

  for (int i = 0; i < anzahlMessungen; i++) {
    summe += analogRead(LDR_PIN);
    delay(50);
  }
  referenzHelligkeit = summe / anzahlMessungen;
  Serial.print("Neue Referenzhelligkeit: ");
  Serial.println(referenzHelligkeit);
  letzteKalibrierung = millis();
}

int readLDR() {
  return analogRead(LDR_PIN);
}

bool isDark() {
  int lichtwert = readLDR();

  // Prüfe, ob es Zeit für Kalibrierung ist UND es nicht dunkel ist
  if ((millis() - letzteKalibrierung) > kalibrierIntervall && lichtwert > (referenzHelligkeit * faktor)) {
    kalibriereLDR();
  }

  int threshold = referenzHelligkeit * faktor;
  return (lichtwert < threshold);
}