// ============================================
// MARCHA PERONISTA - Melodía para Buzzer
// Transcripción simplificada de notas principales
// ============================================

#ifndef MARCHA_PERONISTA_H
#define MARCHA_PERONISTA_H

// ========== DEFINICIÓN DE NOTAS (Frecuencias en Hz) ==========
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST     0

// ========== MARCHA PERONISTA - VERSIÓN 8-BIT EXTRAÍDA ==========
// Melodía extraída de versión 8-bit/chiptune
// 76 notas - Debería sonar mejor en el buzzer
// Tonalidad: Do# menor (C#m)

// Melodía principal (notas) - EXTRAÍDA DE VERSIÓN 8-BIT
int marchaPeron_melody[] = {
  NOTE_D3, NOTE_CS3, NOTE_FS3, NOTE_CS3, NOTE_FS3, NOTE_A3, NOTE_CS4, NOTE_D4, 
  NOTE_CS4, NOTE_CS4, NOTE_CS4, NOTE_CS4, NOTE_FS4, NOTE_FS4, NOTE_A4, NOTE_CS4, 
  NOTE_B4, NOTE_A4, NOTE_GS4, NOTE_CS4, NOTE_FS4, NOTE_CS4, NOTE_FS4, NOTE_CS4, 
  NOTE_A3, NOTE_CS4, NOTE_A3, NOTE_CS4, NOTE_A3, NOTE_CS4, NOTE_A3, NOTE_CS4, 
  NOTE_A3, NOTE_CS4, NOTE_CS5, NOTE_A4, NOTE_FS4, NOTE_CS5, NOTE_A4, NOTE_FS4, 
  NOTE_CS5, NOTE_A4, NOTE_CS5, NOTE_A4, NOTE_FS4, NOTE_CS5, NOTE_B4, NOTE_A4, 
  NOTE_B4, NOTE_GS4, NOTE_B4, NOTE_GS4, NOTE_F4, NOTE_B4, NOTE_GS4, NOTE_F4, 
  NOTE_B4, NOTE_GS4, NOTE_D5, NOTE_CS5, NOTE_B4, NOTE_CS5, NOTE_D5, NOTE_CS5, 
  NOTE_B4, NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_B4, NOTE_CS5, NOTE_A4, NOTE_FS4, 
  NOTE_CS5, NOTE_A4, NOTE_FS4, NOTE_CS5
};

// Duraciones de cada nota (en milisegundos) - EXTRAÍDAS DE 8-BIT
int marchaPeron_durations[] = {
  208, 116, 232, 255, 232, 232, 719, 116, 
  835, 139, 185, 278, 208, 232, 208, 417, 
  441, 464, 464, 232, 255, 348, 232, 185, 
  301, 185, 278, 185, 278, 208, 278, 162, 
  325, 185, 487, 371, 116, 487, 348, 116, 
  975, 952, 487, 348, 116, 487, 371, 116, 
  952, 952, 487, 371, 116, 487, 348, 116, 
  952, 975, 1439, 348, 139, 232, 232, 255, 
  232, 232, 255, 232, 232, 487, 348, 139, 
  464, 371, 116, 185
};

// Número de notas en la melodía
const int marchaPeron_length = 76;

// ========== VERSIÓN ALTERNATIVA MÁS CORTA ==========
// Solo primera frase para alarmas menos invasivas

int marchaPeron_short[] = {
  // "Los muchachos peronistas todos unidos"
  NOTE_E4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4,
  NOTE_G4, NOTE_G4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_B4, NOTE_FS4
};

int marchaPeron_short_durations[] = {
  272, 272, 272, 272, 545, 272, 272, 545,
  272, 272, 272, 272, 272, 272, 545, 545
};

const int marchaPeron_short_length = sizeof(marchaPeron_short) / sizeof(marchaPeron_short[0]);

#endif // MARCHA_PERONISTA_H
