#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include "display_menu.h"
#include <DFRobotDFPlayerMini.h>
#include "light_mapping.h"
#include "sound_to_vibe.h"
#include "effects_control.h"

extern DFRobotDFPlayerMini dfplayer;
extern bool systemAktiv; // Systemstatus prüfen

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define MAX_SONGS 20
String songIDs[MAX_SONGS];
String songNames[MAX_SONGS];
int songCount = 0;
int selectedIndex = 0;
bool songSelected = false;
int lastSelectedIndex = -1;


/*
initialisiert das Display und schaltet das Backlight an.
*/
void setupMenuDisplay() {
  lcd.init();
  lcd.backlight();
}

/*
liest die Datei songs.txt von der SD-Karte ein und füllt die Arrays songIDs[] und songNames[].
*/
void loadSongList() {
  Serial.println("loadSongList() gestartet");
  File f = SD.open("songs.txt");
  if (!f) {
    Serial.println("❌ songs.txt nicht gefunden!");
    return;
  }
  songCount = 0;
  while (f.available() && songCount < MAX_SONGS) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;
    int sep = line.indexOf(',');
    if (sep < 0) continue;
    songIDs[songCount] = line.substring(0, sep);
    songNames[songCount] = line.substring(sep + 1);
    
    songCount++;
    // Debug-Ausgabe zur Überprüfung
    Serial.print("Song geladen: ");
    Serial.println(songNames[songCount - 1]);
  }
  f.close();
  Serial.print("✅ Songs geladen: ");
  Serial.println(songCount);
}
void scrollTextRight(String text, int row, int delayTime = 300) {
  int visibleLength = 16;                // Anzahl sichtbarer Zeichen auf dem LCD
  int textLength = text.length();        // Gesamtlänge des Textes

  // Wenn der Text kurz genug ist, einfach anzeigen
  if (textLength <= visibleLength) {
    lcd.setCursor(0, row);
    lcd.print(text);
    return;
  }

  // Schritt 1: Text nach rechts einblenden (von rechts nach links auffüllen)
  for (int i = visibleLength; i > 0; i--) {
    lcd.setCursor(0, row);
    lcd.print(text.substring(0, visibleLength - i));  // Teilstring anzeigen
    lcd.print("                ");                    // Rest löschen
    delay(delayTime);
  }

  // Schritt 2: Text nach rechts durchlaufen lassen
  for (int i = 0; i <= textLength - visibleLength; i++) {
    lcd.setCursor(0, row);
    lcd.print(text.substring(i, i + visibleLength));  // Sichtbarer Abschnitt
    delay(delayTime);
  }

  // Schritt 3: Am Ende langsam nach rechts „herauslaufen“ lassen
  for (int i = 1; i <= visibleLength; i++) {
    lcd.setCursor(0, row);
    lcd.print(text.substring(textLength - visibleLength + i));
    lcd.print("                ");  // Auffüllen mit Leerzeichen
    delay(delayTime);
  }
}


/*
zeigt auf dem LCD das aktuelle ausgewählte Lied an, wenn systemAktiv true ist. Sonst wird das Backlight abgeschaltet.
*/
void updateDisplay() {
  if (systemAktiv) {
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Now playing:    ");  // Leerzeichen, um alte Zeichen zu löschen
    
    //lcd.setCursor(0, 1);
    //lcd.print(songNames[selectedIndex]);
    scrollTextRight(songNames[selectedIndex], 1,200);
    
    int len = songNames[selectedIndex].length();
    for (int i = len; i < 16; i++) {
      lcd.print(" ");  // Rest der Zeile löschen
    }
  } else {
    lcd.clear();
    lcd.noBacklight();
  }
}


void startFirstSong() {
  selectedIndex = 0;    // Erster Song
  Serial.println("startFirstSong() aufgerufen!"); 
  playSelectedSong();   // Startet Musik + zeigt Display an
}


/*
liest den Song aus songIDs[selectedIndex] und startet ihn mit dfplayer.play(songNumber)
*/
void playSelectedSong() {
  int songNumber = songIDs[selectedIndex].toInt();

  dfplayer.stop();
  delay(100);
  dfplayer.play(songNumber);
  delay(100);
  //updateDisplay();

  // --- HIER Mapping-Dateien dynamisch laden ---
  String lightFile = songIDs[selectedIndex] + "LT.csv";
  String vibeFile = songIDs[selectedIndex] + "V.csv";

  if (!loadLightMapping(lightFile.c_str())) {
    Serial.println("⚠️ Lichtmapping konnte nicht geladen werden!");
  }
  if (!loadVibrationMapping(vibeFile.c_str())) {
    Serial.println("⚠️ Vibrationsmapping konnte nicht geladen werden!");
  }
  // ---------------------------------------------

  resetAllEffects();  // Zeitbasis für Licht + Vibration synchron zurücksetzen

  
  updateLights();    // Einmaliges Update direkt nach Reset
  updateVibration();
  updateDisplay();
  

/*

  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Now playing:");
  lcd.setCursor(0, 1);
  lcd.print(songNames[selectedIndex]);
  songSelected = true;
  */
}




/*
liest die Tasten (Pin 8 = Hoch, 5 = Runter, 7 = OK) aus und ändert den selectedIndex. Wenn sich die Auswahl ändert, wird sofort playSelectedSong() aufgerufen.
*/

// globale Variablen (in display_menu.cpp)
bool lastButtonUpState = HIGH;
bool lastButtonDownState = HIGH;

void handleMenuButtons() {
  bool currentUp = digitalRead(8);
  bool currentDown = digitalRead(5);

  // Flanke: Taste gedrückt (HIGH -> LOW)
  if (lastButtonUpState == HIGH && currentUp == LOW) {
    selectedIndex--;
    if (selectedIndex < 0) selectedIndex = songCount - 1;
    playSelectedSong();
    delay(50);  // Entprellung
  }

  if (lastButtonDownState == HIGH && currentDown == LOW) {
    selectedIndex++;
    if (selectedIndex >= songCount) selectedIndex = 0;
    playSelectedSong();
    delay(50);
  }

  lastButtonUpState = currentUp;
  lastButtonDownState = currentDown;
}
