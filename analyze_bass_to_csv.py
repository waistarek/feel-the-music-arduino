import librosa
import numpy as np
import argparse
import sys
import csv

def main():
    parser = argparse.ArgumentParser(description="Bass-Events aus einer WAV-Datei extrahieren und als CSV speichern.")
    parser.add_argument('input_wav', help='Pfad zur Eingabe-WAV-Datei')
    parser.add_argument('output_csv', help='Name der Ausgabedatei (.csv)')
    parser.add_argument('--bass_freq_limit', type=float, default=150, help='Maximale Bassfrequenz in Hz (Standard: 150)')
    parser.add_argument('--threshold_percentile', type=float, default=90, help='Schwellenwert-Perzentil (Standard: 90)')
    parser.add_argument('--min_distance_ms', type=int, default=200, help='Mindestabstand zwischen Events in ms (Standard: 200)')
    parser.add_argument('--sampling_interval', type=int, default=250, help='Abstand zwischen Ausgabewerten in ms (Standard: 250)')
    args = parser.parse_args()

    try:
        y, sr = librosa.load(args.input_wav, sr=None, mono=True)
    except Exception as e:
        print(f"Fehler beim Laden der Datei: {e}")
        sys.exit(1)

    stft = np.abs(librosa.stft(y, n_fft=2048, hop_length=512))
    frequencies = librosa.fft_frequencies(sr=sr)

    bass_indices = np.where(frequencies < args.bass_freq_limit)[0]
    bass_energy = stft[bass_indices, :].sum(axis=0)

    times = librosa.frames_to_time(np.arange(len(bass_energy)), sr=sr, hop_length=512)
    threshold = np.percentile(bass_energy, args.threshold_percentile)
    events = np.round(times[bass_energy > threshold] * 1000).astype(int)

    filtered = []
    last = -args.min_distance_ms
    for t in events:
        if t - last >= args.min_distance_ms:
            filtered.append(t)
            last = t

    # Sampling reduzieren
    sampled = []
    last_out = -args.sampling_interval
    for t in filtered:
        if t - last_out >= args.sampling_interval:
            sampled.append(t)
            last_out = t

    try:
        with open(args.output_csv, "w", newline='') as f:
            writer = csv.writer(f)
            writer.writerow(["timestamp", "vibration"])
            for t in sampled:
                writer.writerow([t, 100])  # konstante Vibrationsstärke
        print(f"CSV erfolgreich gespeichert in: {args.output_csv}")
    except Exception as e:
        print(f"Fehler beim Schreiben der Datei: {e}")

if __name__ == "__main__":
    main()
