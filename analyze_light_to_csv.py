import librosa
import numpy as np
import argparse
import sys
import csv
from colorsys import hsv_to_rgb

def frequency_to_color(freq, freq_min=50, freq_max=8000):
    norm = (freq - freq_min) / (freq_max - freq_min)
    norm = max(0.0, min(1.0, norm))
    h = norm
    s = 1.0
    v = 1.0
    r, g, b = hsv_to_rgb(h, s, v)
    return (int(r * 255), int(g * 255), int(b * 255))

def scale_brightness(rgb, volume):
    factor = min(max(volume, 0.0), 1.0)
    return tuple(int(c * factor) for c in rgb)

def main():
    parser = argparse.ArgumentParser(description="Erzeuge Licht-Mapping aus Audio (.wav) basierend auf Lautstärke und Frequenz")
    parser.add_argument("input_wav", help="Pfad zur Eingabe-WAV-Datei")
    parser.add_argument("output_csv", help="Name der Ausgabedatei (.csv)")
    parser.add_argument("--hop_ms", type=int, default=100, help="Analyseintervall in Millisekunden (Standard: 100ms)")
    parser.add_argument("--sampling_interval", type=int, default=250, help="Zeitabstand zwischen Einträgen in ms (Standard: 250ms)")
    args = parser.parse_args()

    try:
        y, sr = librosa.load(args.input_wav, sr=None, mono=True)
    except Exception as e:
        print(f"Fehler beim Laden der Datei: {e}")
        sys.exit(1)

    hop_length = int(sr * args.hop_ms / 1000)
    stft = np.abs(librosa.stft(y, n_fft=2048, hop_length=hop_length))
    freqs = librosa.fft_frequencies(sr=sr)
    times = librosa.frames_to_time(np.arange(stft.shape[1]), sr=sr, hop_length=hop_length)

    output = []
    last_time = -args.sampling_interval

    for i in range(stft.shape[1]):
        time_ms = int(times[i] * 1000)
        if time_ms - last_time < args.sampling_interval:
            continue

        spectrum = stft[:, i]
        if len(spectrum) == 0:
            continue
        max_idx = np.argmax(spectrum)
        max_freq = freqs[max_idx]
        rms = np.sqrt(np.mean(spectrum**2))
        norm_volume = min(rms / np.max(stft), 1.0) if np.max(stft) > 0 else 0

        base_color = frequency_to_color(max_freq)
        rgb = scale_brightness(base_color, norm_volume)
        output.append((time_ms, *rgb))
        last_time = time_ms

    try:
        with open(args.output_csv, "w", newline='') as f:
            writer = csv.writer(f)
            writer.writerow(["timestamp", "red", "green", "blue"])
            writer.writerows(output)
        print(f"CSV erfolgreich gespeichert in: {args.output_csv}")
    except Exception as e:
        print(f"Fehler beim Schreiben der Datei: {e}")

if __name__ == "__main__":
    main()
