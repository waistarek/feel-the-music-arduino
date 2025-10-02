import os
import subprocess
import argparse

def main():
    parser = argparse.ArgumentParser(description="Erzeuge alle Mapping-Dateien (.csv) + Songliste für eine gegebene ID und WAV-Datei.")
    parser.add_argument("id", help="Numerische ID für den Song (z. B. 002)")
    parser.add_argument("input_wav", help="Pfad zur WAV-Datei (z. B. UptownFunk.wav)")
    parser.add_argument("display_name", help="Anzeigename für das Display (z. B. 'Uptown Funk')")
    args = parser.parse_args()

    # Dateinamen automatisch erzeugen
    id = args.id
    wav = args.input_wav
    light_csv = f"{id}LT.csv"
    vibration_csv = f"{id}V.csv"

    # Aufruf der Mapping-Skripte
    print(f"[1/3] Erzeuge Vibrationsmapping --> {vibration_csv}")
    subprocess.run(["python", "analyze_bass_to_csv.py", wav, vibration_csv], check=True)

    print(f"[2/3] Erzeuge Lichtmapping --> {light_csv}")
    subprocess.run(["python", "analyze_light_to_csv.py", wav, light_csv], check=True)

    # songs.txt aktualisieren
    print(f"[3/3] Aktualisiere songs.txt mit ID '{id}' und Name '{args.display_name}'")

    song_line = f"{id},{args.display_name}\n"
    if os.path.exists("songs.txt"):
        with open("songs.txt", "r+", encoding="utf-8") as f:
            lines = f.readlines()
            if any(line.startswith(f"{id},") for line in lines):
                print(f"⚠️  Eintrag mit ID {id} existiert bereits in songs.txt – wird nicht doppelt eingefügt.")
            else:
                f.write(song_line)
    else:
        with open("songs.txt", "w", encoding="utf-8") as f:
            f.write(song_line)

    print("OK: Alle Dateien wurden erfolgreich erzeugt.")

if __name__ == "__main__":
    main()
