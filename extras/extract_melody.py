#!/usr/bin/env python3
"""
Script para extraer la melodía de un MP3 y convertirla a notas para buzzer
Usa librosa para análisis de audio
"""

import numpy as np
import sys
import librosa

# Mapeo de frecuencias a notas
NOTE_NAMES = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B']

def freq_to_note(freq):
    """Convierte frecuencia Hz a nombre de nota y octava"""
    if freq <= 0:
        return None, None
    
    # A4 = 440 Hz es la referencia
    A4 = 440
    C0 = A4 * pow(2, -4.75)  # Frecuencia de C0
    
    # Calcular el número de semitonos desde C0
    h = 12 * np.log2(freq / C0)
    octave = int(h / 12)
    n = int(round(h % 12)) % 12  # Asegurar que n esté entre 0-11
    
    return NOTE_NAMES[n], octave

def freq_to_arduino_note(freq):
    """Convierte frecuencia a nombre de nota en formato Arduino (ej: NOTE_C4)"""
    note_name, octave = freq_to_note(freq)
    if note_name is None:
        return "REST"
    
    # Formato: NOTE_C4, NOTE_CS4, etc.
    arduino_name = note_name.replace('#', 'S')
    return f"NOTE_{arduino_name}{octave}"

def extract_melody(mp3_file, output_file, duration=30, tempo_bpm=110):
    """Extrae la melodía principal de un MP3"""
    
    print(f"Analizando: {mp3_file}")
    print("Esto puede tomar un momento...")
    
    # Cargar audio (solo primeros 30 segundos)
    y, sr = librosa.load(mp3_file, duration=duration, sr=22050)
    
    print(f"Audio cargado: {len(y)/sr:.2f} segundos, sample rate={sr}")
    
    # Detectar pitch (tono) usando piptrack
    pitches, magnitudes = librosa.piptrack(y=y, sr=sr, fmin=100, fmax=1000)
    
    # Extraer la frecuencia dominante en cada frame
    hop_length = 512
    times = librosa.frames_to_time(np.arange(pitches.shape[1]), sr=sr, hop_length=hop_length)
    
    # Obtener pitch dominante
    pitch_values = []
    for t in range(pitches.shape[1]):
        index = magnitudes[:, t].argmax()
        pitch = pitches[index, t]
        if pitch > 0:
            pitch_values.append(pitch)
        else:
            pitch_values.append(0)
    
    # Filtrar y limpiar: agrupar notas similares
    # Calcular duración de cada nota en ms
    ms_per_beat = 60000 / tempo_bpm  # ms por beat
    frame_duration = hop_length / sr * 1000  # ms por frame
    
    # Agrupar frames en notas
    notes = []
    durations = []
    
    current_freq = 0
    current_duration = 0
    threshold_cents = 50  # Umbral de diferencia (medio tono)
    
    for i, freq in enumerate(pitch_values):
        if freq > 0:
            # Si es similar a la nota actual, acumular duración
            if current_freq == 0:
                current_freq = freq
                current_duration = frame_duration
            elif abs(1200 * np.log2(freq / current_freq)) < threshold_cents:
                current_duration += frame_duration
            else:
                # Nueva nota diferente
                if current_duration > 100:  # Filtrar notas muy cortas
                    notes.append(current_freq)
                    durations.append(int(current_duration))
                current_freq = freq
                current_duration = frame_duration
        else:
            # Silencio
            if current_freq > 0 and current_duration > 100:
                notes.append(current_freq)
                durations.append(int(current_duration))
            current_freq = 0
            current_duration = 0
    
    # Agregar última nota
    if current_freq > 0 and current_duration > 100:
        notes.append(current_freq)
        durations.append(int(current_duration))
    
    print(f"\nNotas detectadas: {len(notes)}")
    
    # Limitar a primeras 60-80 notas (suficiente para una frase)
    notes = notes[:80]
    durations = durations[:80]
    
    # Generar código Arduino
    with open(output_file, 'w') as f:
        f.write("// Melodía extraída de: " + mp3_file + "\n")
        f.write(f"// Total de notas: {len(notes)}\n\n")
        
        f.write("int marchaPeron_melody_extracted[] = {\n  ")
        for i, freq in enumerate(notes):
            note = freq_to_arduino_note(freq)
            f.write(note)
            if i < len(notes) - 1:
                f.write(", ")
            if (i + 1) % 8 == 0:
                f.write("\n  ")
        f.write("\n};\n\n")
        
        f.write("int marchaPeron_durations_extracted[] = {\n  ")
        for i, dur in enumerate(durations):
            f.write(str(dur))
            if i < len(durations) - 1:
                f.write(", ")
            if (i + 1) % 8 == 0:
                f.write("\n  ")
        f.write("\n};\n\n")
        
        f.write(f"const int marchaPeron_extracted_length = {len(notes)};\n")
    
    print(f"\n✓ Código generado en: {output_file}")
    print(f"✓ {len(notes)} notas extraídas")
    
    # Mostrar primeras 20 notas para verificar
    print("\nPrimeras 20 notas:")
    for i in range(min(20, len(notes))):
        note_name, octave = freq_to_note(notes[i])
        print(f"  {i+1}. {note_name}{octave} ({notes[i]:.1f}Hz) - {durations[i]}ms")

if __name__ == "__main__":
    mp3_file = "include/marcha_peronista.mp4"  # Versión mejorada MP4
    output_file = "marcha_extracted.txt"
    
    print("=" * 60)
    print("Extractor de Melodía - Marcha Peronista")
    print("=" * 60)
    
    try:
        extract_melody(mp3_file, output_file, duration=30, tempo_bpm=110)
        print("\n✓ Extracción completada!")
        print(f"\nRevisa el archivo '{output_file}' y copia el código")
        print("a 'include/marcha_peronista.h'")
    except FileNotFoundError:
        print(f"\n✗ Error: No se encontró {mp3_file}")
        print("Asegúrate de que el archivo MP3 existe.")
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()
