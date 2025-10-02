#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <DFRobotDFPlayerMini.h>

#include "display_menu.h"
#include "light_mapping.h"     // LED-Mapping
#include "sound_to_vibe.h"     // Vibrations-Mapping
#include "ldr_sensor.h" 

#define SD_CS_PIN 4
#define DF_RX 0
#define DF_TX 1

DFRobotDFPlayerMini dfplayer;
bool systemAktiv = false;       // System startet AUS
bool lastButtonState = LOW;
bool musikGestartet = false; 
extern unsigned long startTime;
extern unsigned long startTime_vibe;
extern int lastSelectedIndex;
extern int selectedIndex;


void setup() {
  Serial.begin(9600);
  delay(1000);

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("❌ SD-Karte nicht erkannt!");
    while (true);
  }
  Serial.println("✅ SD-Karte bereit.");

// 🎵 DFPlayer über Hardware-Serial starten
  Serial1.begin(9600);
  if (!dfplayer.begin(Serial1)) {
    Serial.println("❌ DFPlayer nicht erkannt!");
    while (true);
  }
  Serial.println("✅ DFPlayer erkannt!");
  dfplayer.volume(80);

  setupLights();
  setupVibration();
  setupMenuDisplay();
  loadSongList();
  updateDisplay();

  if (!loadLightMapping("001LT.csv")) {
    Serial.println("⚠️ Lichtmapping konnte nicht geladen werden!");
  }
  if (!loadVibrationMapping("001V.csv")) {
    Serial.println("⚠️ Vibrationsmapping konnte nicht geladen werden!");
  }

  pinMode(8, INPUT); // Hoch (grün)
  pinMode(5, INPUT); // Runter (Gelb)
  //pinMode(7, INPUT_PULLUP); // OK (blau)
  pinMode(9, INPUT);        // EIN/AUS-Taste ohne Pullup
  setupLDR(); // Lichtsensor


  Serial.println("🔧 Setup abgeschlossen. Warte auf Einschalten...");
}

void loop() {
  /*
  int hoch = digitalRead(8);
  int runter = digitalRead(5);
  Serial.print("Pin 8: "); Serial.print(hoch);
  Serial.print(" | Pin 5: "); Serial.println(runter);
  delay(200);
  */
  //Lichtsenstor Debug
  //int lichtwert = analogRead(A0);
  //Serial.print("Lichtwert: ");
  //Serial.println(lichtwert);
  //Serial.print("isDark: ");
  //Serial.println(isDark() ? "true" : "false");
  bool currentButton = digitalRead(9);  // Aktuellen Tasterstatus lesen

  // Wenn der Taster gerade gedrückt wurde (von LOW zu HIGH)
  if (currentButton == HIGH && lastButtonState == LOW) {
    systemAktiv = !systemAktiv;  // Systemstatus umschalten
    Serial.print("System ist jetzt: ");
    Serial.println(systemAktiv ? "🟢 AKTIV" : "🔴 INAKTIV");

    if (systemAktiv) {
      Serial.println("System aktiviert: loadSongList() aufrufen");
      // Wenn das System aktiviert wird, lade und zeige den ersten Song an
      loadSongList();   // Songs von der SD-Karte laden
      
      musikGestartet = false;
      lastSelectedIndex = -1; // Reset

    } else {
      Serial.println("System deaktiviert: Musik stoppen, Display löschen");
      lcd.clear();      // Lösche das Display, wenn das System deaktiviert wird
      lcd.noBacklight(); // Hintergrundbeleuchtung ausschalten
      dfplayer.stop();
      resetLights();
      resetVibration();
      musikGestartet = false;
    }

    while (digitalRead(9) == HIGH) delay(10); // Warte, bis der Knopf losgelassen wird
  }

  lastButtonState = currentButton;  // Speichere den aktuellen Zustand des Knopfes

  // Weitere Logik für das System
  if (systemAktiv) {
    if (!musikGestartet) {
      if(isDark()){
        Serial.println("Dunkelheit erkannt, starte Musik...");
        startFirstSong();      // startet erstes Lied + Display
        musikGestartet = true;
        updateDisplay();
        //lastSelectedIndex = selectedIndex;
        //startTime = millis();
        //startTime_vibe = millis();
      }
    }

    if (musikGestartet) { 
      /*
      if(digitalRead(8) == LOW){
        Serial.print("🔘 Zustand von Pin 8: ");
        Serial.println(digitalRead(8));
        handleMenuButtons();
      }
      if(digitalRead(5) == LOW){
        Serial.print("🔘 Zustand von Pin 5: ");
        Serial.println(digitalRead(5));
        handleMenuButtons();
      }
      */
      handleMenuButtons();
      //updateDisplay();
      updateLights();
      updateVibration(); 
      
    }
  } else {
    resetLights();
    resetVibration();
    musikGestartet = false;
  }

  delay(100);
}
