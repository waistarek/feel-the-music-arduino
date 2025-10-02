#pragma once
// Externe Deklaration der LCD-Instanz
extern LiquidCrystal_I2C lcd;

// Gibt den globalen Status zurück, ob bereits ein Song gespielt wurde
extern bool songSelected;

// Externe Deklaration für displayAktiv
extern bool displayAktiv;

// Initialisiert das LCD-Menüsystem
void setupMenuDisplay();

// Liest alle Songdaten aus songs.txt
void loadSongList();

// Zeigt den aktuell gewählten Song im Menü an
void updateDisplay();

// Behandelt Taster-Eingaben (hoch, runter, OK)
void handleMenuButtons();

// Spielt den aktuell ausgewählten Song über DFPlayer ab
void playSelectedSong();

// Spielt automatisch Song 1, wenn z. B. Hand auf Sensor erkannt wird
void playFirstSongFromSensor();
void startFirstSong();



