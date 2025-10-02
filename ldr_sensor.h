#pragma once

// Initialisiert den LDR-Sensor (z.B. Pin setzen, evtl. Kalibrierung)
void setupLDR();

// Liest den aktuellen Lichtwert zurück (0-1023)
int readLDR();

// Prüft, ob es dunkel genug ist, um Effekte zu starten (Threshold intern genutzt)
bool isDark();
void kalibriereLDR();
